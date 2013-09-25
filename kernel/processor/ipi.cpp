/**
 * Reaver Project OS, Rose License
 *
 * Copyright (C) 2013 Reaver Project Team:
 * 1. Michał "Griwes" Dominiak
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation is required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * Michał "Griwes" Dominiak
 *
 **/

#include <atomic>

#include <processor/ipi.h>
#include <processor/handlers.h>
#include <scheduler/thread.h>

namespace
{
    struct _parallel_slot
    {
        uint8_t int_vector = 0;
        utils::spinlock lock;
        void (*fptr)(uint64_t) = nullptr;
        uint64_t data = 0;
        std::atomic<uint64_t> unfinished_cores{ 0 };
    } _slots[8];

    std::atomic<uint64_t> _next_slot{ 0 };

    void _interrupt_handler(processor::isr_context & isrc, uint64_t i)
    {
        if (likely(scheduler::ready()))
        {
            scheduler::current_thread()->save(isrc);
        }

        _parallel_slot & slot = _slots[i];

        slot.fptr(slot.data);
        --slot.unfinished_cores;

        if (likely(scheduler::ready()))
        {
            scheduler::current_thread()->load(isrc);
        }
    }
}

void processor::smp::parallel_execute(processor::smp::policies policy, void (*fptr)(uint64_t), uint64_t data, uint64_t target)
{
    _parallel_slot & slot = _slots[_next_slot++ % 8];

    LOCK(slot.lock);

    slot.fptr = fptr;

    switch (policy)
    {
        case policies::all:
            slot.unfinished_cores = get_core_count();
            broadcast(broadcasts::others, ipis::generic, slot.int_vector);
            fptr(data);
            --slot.unfinished_cores;
            break;

        case policies::others:
            slot.unfinished_cores = get_core_count() - 1;
            broadcast(broadcasts::others, ipis::generic, slot.int_vector);
            break;

        case policies::others_no_wait:
            slot.unfinished_cores = 0;
            broadcast(broadcasts::others, ipis::generic, slot.int_vector);
            break;

        case policies::specific:
            slot.unfinished_cores = 1;
            ipi(target, ipis::generic, slot.int_vector);
            break;
    }

    while (slot.unfinished_cores)
    {
        asm volatile ("pause");
    }

    if (policy != policies::others_no_wait)
    {
        slot.fptr = nullptr;
        slot.data = 0;
    }
}

void processor::smp::initialize_parallel()
{
    for (uint8_t i = 0; i < 8; ++i)
    {
        _slots[i].int_vector = allocate_isr(1);
        register_handler(_slots[i].int_vector, _interrupt_handler, i);
    }
}

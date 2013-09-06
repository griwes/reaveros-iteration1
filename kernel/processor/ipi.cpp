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

#include <processor/ipi.h>
#include <processor/handlers.h>

namespace
{
    uint8_t _int_vector = 0;

    utils::spinlock _lock;
    void (*_fptr)(uint64_t) = nullptr;
    uint64_t _data = 0;

    uint64_t _num_cores_finished = 0;

    void _interrupt_handler(processor::idt::isr_context, uint64_t)
    {
        _fptr(_data);
        ++_num_cores_finished;
    }
}

void processor::parallel_execute(void (*fptr)(uint64_t), uint64_t data)
{
    LOCK(_lock);

    if (!_int_vector)
    {
        _int_vector = allocate_isr(0);
        register_handler(_int_vector, _interrupt_handler);
    }

    _fptr = fptr;
    broadcast(broadcasts::others, ipis::generic, _int_vector);
    fptr(data);
    ++_num_cores_finished;

    while (_num_cores_finished < get_core_count())
    {
        asm volatile ("pause");
    }

    _fptr = nullptr;
    _data = 0;
    _num_cores_finished = 0;
}

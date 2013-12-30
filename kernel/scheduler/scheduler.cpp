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

#include <scheduler/scheduler.h>
#include <processor/ipi.h>
#include <processor/core.h>
#include <scheduler/process.h>
#include <scheduler/thread.h>
#include <scheduler/manager.h>
#include <processor/thread.h>

namespace
{
    scheduler::manager<scheduler::process> _pcb_manager{ nullptr };
    scheduler::manager<scheduler::thread> _tcb_manager{ nullptr };

    int64_t _score(scheduler::thread * t, processor::core * c)
    {
        int64_t score = 1024;

        if (t->last_core == c)
        {
            score += 1024;
        }

        score -= c->scheduler().load();

        return score;
    }

    void _schedule(scheduler::thread * t)
    {
        processor::core * best_core = processor::get_cores();
        int64_t best_score = _score(t, best_core);

        for (uint64_t i = 1; i < processor::get_core_count(); ++i)
        {
            int64_t score = _score(t, processor::get_cores() + i);

            if (score > best_score)
            {
                best_core = processor::get_cores() + i;
                best_score = score;
            }
        }

        best_core->scheduler().push(t);
    }

    bool _ready = false;
}

void scheduler::initialize()
{
    screen::debug("\nInitializing PCB manager at ", &_pcb_manager);
    new (&_pcb_manager) manager<process>{};
    screen::debug("\nInitializing TCB manager at ", &_tcb_manager);
    new (&_tcb_manager) manager<thread>{};

    processor::smp::parallel_execute([](uint64_t){ ap_initialize(); });

    _ready = true;
}

void scheduler::ap_initialize()
{
    INTL();

    screen::debug("\nInitializing local thread scheduler on core #", processor::id());
    new (&processor::get_current_core()->scheduler()) local{};

    thread * kernel_thread = _tcb_manager.allocate();
    kernel_thread->last_core = processor::get_current_core();
    kernel_thread->address_space = processor::get_asid();
    processor::get_current_core()->scheduler().push(kernel_thread);
    processor::set_current_thread(kernel_thread);

    screen::debug("\nLocal kernel thread ID on core #", processor::id(), ": ", kernel_thread->id);
}

bool scheduler::ready()
{
    return _ready;
}

scheduler::thread * scheduler::current_thread()
{
    return processor::current_thread();
}

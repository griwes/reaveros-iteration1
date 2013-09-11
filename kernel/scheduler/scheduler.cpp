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
#include <scheduler/process.h>
#include <scheduler/thread.h>
#include <scheduler/manager.h>

namespace
{
    scheduler::manager<scheduler::process> _pcb_manager{ nullptr };
    scheduler::manager<scheduler::thread> _tcb_manager{ nullptr };
}

void scheduler::initialize()
{
    screen::debug("\nInitializing PCB manager at ", &_pcb_manager);
    new (&_pcb_manager) manager<process>{};
    screen::debug("\nInitializing TCB manager at ", &_tcb_manager);
    new (&_tcb_manager) manager<thread>{};

    processor::smp::parallel_execute([](uint64_t){ ap_initialize(); });
}

void scheduler::ap_initialize()
{
}

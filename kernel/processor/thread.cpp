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

#include <processor/thread.h>
#include <processor/core.h>
#include <scheduler/thread.h>

void processor::set_current_thread(scheduler::thread * thread)
{
    auto previous = processor::current_thread();
    processor::core * core;

    if (previous)
    {
        core = previous->current_core;
        previous->current_core = nullptr;
        previous->last_core = core;
    }

    else
    {
        core = processor::get_current_core();
    }

    processor::set_asid(thread->address_space);
    core->thread = thread;
}

scheduler::thread * processor::current_thread()
{
    scheduler::thread * ret = nullptr;
    asm volatile("mov %%gs:0, %0" : "=r"(ret));
    return ret;
}

/**
 * Reaver Project OS, Rose License
 *
 * Copyright (C) 2013-2014 Reaver Project Team:
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

#pragma once

#include <utils/allocator.h>
#include <scheduler/scheduler.h>

namespace user
{
    class user;
}

namespace scheduler
{
    struct thread;
    class mailbox;

    struct process : public utils::chained<process>
    {
        utils::spinlock lock;

        uint64_t id;

        user::user * owner = nullptr;
        process * parent = nullptr;
        process * child = nullptr;

        thread * main_thread = nullptr;

        mailbox * box = nullptr;

        uint64_t address_space = 0;
        uint64_t per_thread_foreign:1;
        uint64_t zombie:1;

        uint64_t exit_value;

        scheduling_policy policy = scheduling_policy::normal;
        uint8_t priority = 128;
    };
}

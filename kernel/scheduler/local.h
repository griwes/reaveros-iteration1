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

#pragma once

#include <scheduler/thread.h>
#include <utils/vfq.h>
#include <scheduler/scheduler.h>
#include <time/timer.h>

namespace scheduler
{
    class local
    {
    public:
        local(decltype(nullptr))
        {
        }

        local() : _core{ processor::id() }
        {
        }

        void push(thread *);
        void remove(thread *);

        void do_switch();

        uint64_t load()
        {
            return 4 * _top.load() + 2 * _normal.load() + _background.load();
        }

    private:
        thread * _pop();
        void _do_switch();

        uint64_t _core;

        utils::variable_frequency_queue<thread *, 256> _top;
        utils::variable_frequency_queue<thread *, 256> _normal;
        utils::variable_frequency_queue<thread *, 256> _background;

        time::timer_event_handle _timer;

        utils::spinlock _lock;
    };
}

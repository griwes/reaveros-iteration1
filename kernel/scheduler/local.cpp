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

#include <scheduler/local.h>
#include <processor/ipi.h>
#include <processor/core.h>

namespace
{
    void _set_thread()
    {

    }
}

scheduler::thread * scheduler::local::_pop()
{
    if (_top.load())
    {
        return _top.pop();
    }

    if (_normal.load())
    {
        return _normal.pop();
    }

    return _background.pop();
}

void scheduler::local::push(scheduler::thread * t)
{
    LOCK(_lock);

    switch (t->policy)
    {
        case scheduling_policy::top:
            _top.push(t->priority, t);

            if (_top.size() < 3)
            {
                do_switch();
            }

            return;

        case scheduling_policy::normal:
            _normal.push(t->priority, t);

            if (!_top.size() && _normal.size() < 3)
            {
                do_switch();
            }

            return;

        case scheduling_policy::background:
            _background.push(t->priority, t);

            if (!_top.size() && !_normal.size() && _background.size() < 3)
            {
                do_switch();
            }

            return;
    }
}

void scheduler::local::remove(scheduler::thread * t)
{
    LOCK(_lock);

    switch (t->policy)
    {
        case scheduling_policy::top:
            _top.remove(t);
            return;

        case scheduling_policy::normal:
            _normal.remove(t);
            return;

        case scheduling_policy::background:
            _background.remove(t);
            return;
    }

    if (t == current_thread())
    {
        do_switch();
    }
}

void scheduler::local::do_switch()
{
    LOCK(_lock);

    if (_core == processor::id())
    {
        if (_timer.device)
        {
            _timer.cancel();
        }

        screen::debug("\nRescheduling on core #", _core);

        if (_top.size() > 1 || _normal.size() > 1 || _background.size() > 1)
        {
            _timer = time::preemption_timer()->one_shot(5_ms, [](uint64_t scheduler)
            {
                ((local *)scheduler)->_timer.device = nullptr;
                ((local *)scheduler)->do_switch();
            }, (uint64_t)this);
        }

//        processor::do_switch(_pop());
    }

    else
    {
        processor::smp::parallel_execute(processor::smp::policies::specific, [](uint64_t scheduler)
        {
            ((local *)scheduler)->do_switch();
        }, (uint64_t)this, _core);
    }
}

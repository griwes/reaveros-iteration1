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

#include <processor/timer.h>
#include <processor/idt.h>
#include <screen/screen.h>

namespace
{
    std::atomic<uint64_t> _id;

    processor::timer * _hp_timer = nullptr;
    processor::timer * _sched_timer = nullptr;
}

uint64_t processor::allocate_timer_event_id()
{
    return _id.fetch_add(1);
}

void processor::set_high_precision_timer(processor::timer * t)
{
    _hp_timer = t;
}

void processor::set_scheduling_timer(processor::timer * t)
{
    _sched_timer = t;
}

processor::timer * processor::get_high_precision_timer()
{
    return _hp_timer;
}

processor::timer * processor::get_scheduling_timer()
{
    return _sched_timer;
}

processor::real_timer::real_timer(bool periodic_capable) : _is_periodic{}, _can_periodic{ periodic_capable }, _usage{},
    _active{}, _available{}
{
}

processor::timer_event_handle processor::real_timer::one_shot(uint64_t time, processor::timer_handler handler, uint64_t context)
{
    INTL();
    LOCK(_lock);

    auto desc = _get();

    desc->id = processor::allocate_timer_event_id();
    desc->handler = handler;
    desc->handler_parameter = context;
    desc->time_left = time;

    _add(desc);

    ++_usage;

    return {};
}

processor::timer_event_handle processor::real_timer::periodic(uint64_t period, processor::timer_handler handler, uint64_t context)
{
    INTL();
    LOCK(_lock);

    auto desc = _get();

    desc->id = processor::allocate_timer_event_id();
    desc->handler = handler;
    desc->handler_parameter = context;
    desc->periodic = true;
    desc->period = desc->time_left = period;

    _add(desc);

    _usage += 100;

    return {};
}

void processor::real_timer::cancel(uint64_t id)
{
    INTL();
    LOCK(_lock);

    auto first = _active;

    while (first)
    {
        if (first->id == id)
        {
            if (!first->prev)
            {
                _active = first->next;
            }

            else
            {
                _active->prev->next = _active->next;
            }

            if (first->next)
            {
                first->next->prev = first->prev;
            }

            first->next = _available->next;
            _available = first;

            if (first->periodic)
            {
                _usage -= 100;
            }

            else
            {
                --_usage;
            }

            return;
        }
    }

    PANICEX("Tried to cancel a not active timer.", [&]{
        screen::print("Timer id: ", id);
    });
}

processor::timer_description* processor::real_timer::_get()
{
    auto ret = _available;

    if (!_available)
    {
        _available = allocate_chained<timer_description>();

        ret = _available;
    }

    _available = ret->next;

    if (_available)
    {
        _available->prev = nullptr;
    }

    ret->next = nullptr;
    ret->prev = nullptr;

    return ret;
}

void processor::real_timer::_handle(processor::idt::isr_context isrc)
{
    LOCK(_lock);

    _active->handler(isrc, _active->handler_parameter);

    if (!_is_periodic)
    {
        if (_active->periodic)
        {
            auto fired = _active;
            _active = _active->next;
            _active->prev = nullptr;

            fired->next = nullptr;
            fired->prev = nullptr;
            fired->time_left = fired->period;

            _add(fired);
        }

        else
        {
            auto first_free = _available;
            _available = _active;
            _active = _active->next;
            _available->next = first_free;
            _available->prev = nullptr;
        }

        if (_active)
        {
            if (_active->time_left < 200000)
            {
                _active->next->time_left += 200000 - _active->time_left;
                _active->time_left = 200000;
            }

            _one_shot(_active->time_left);
        }
    }
}

void processor::real_timer::_add(processor::timer_description * desc)
{
    if (desc->periodic && !_active && _can_periodic)
    {
        _is_periodic = true;
        _active = desc;

        _periodic(desc->period);
    }

    else
    {
        TODO;
    }
}

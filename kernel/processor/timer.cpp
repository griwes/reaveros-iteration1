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
    std::atomic<uint64_t> _id = { 0 };

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

processor::real_timer::real_timer(capabilities caps, uint64_t minimal_tick) : _cap{ caps }, _is_periodic{},
    _usage{}, _minimal_tick{ minimal_tick }
{
}

processor::timer_event_handle processor::real_timer::one_shot(uint64_t time, processor::timer_handler handler, uint64_t context)
{
    INTL();
    LOCK(_lock);

    timer_description desc;

    desc.id = processor::allocate_timer_event_id();
    desc.handler = handler;
    desc.handler_parameter = context;
    desc.time_point = _now + time;

    _list.insert(desc);

    ++_usage;

    if (_is_periodic)
    {
        _is_periodic = false;
    }

    if (_cap == capabilities::dynamic || _cap == capabilities::one_shot_capable)
    {
        _one_shot(_list.top()->time_point - _now);
    }

    else if (_cap == capabilities::periodic_capable)
    {
        _periodic(_list.top()->time_point - _now);
    }

    return { this, desc.id };
}

processor::timer_event_handle processor::real_timer::periodic(uint64_t period, processor::timer_handler handler, uint64_t context)
{
    INTL();
    LOCK(_lock);

    timer_description desc;

    desc.id = processor::allocate_timer_event_id();
    desc.handler = handler;
    desc.handler_parameter = context;
    desc.periodic = true;
    desc.period = period;
    desc.time_point = _now + period;

    _list.insert(desc);

    _usage += 100;

    if (_list.size() == 1 && (_cap == capabilities::dynamic || _cap == capabilities::periodic_capable))
    {
        _is_periodic = true;
        _periodic(_list.top()->time_point - _now);
    }

    else
    {
        _is_periodic = false;
        _one_shot(_list.top()->time_point - _now);
    }

    return { this, desc.id };
}

void processor::real_timer::cancel(uint64_t id)
{
    INTL();
    LOCK(_lock);

    bool success = true;
    auto t = _list.remove([&](const timer_description & desc){ return desc.id == id; }, success);

    if (!success)
    {
        PANICEX("Tried to cancel a not active timer.", [&]{
            screen::print("Timer id: ", id);
        });
    }

    if (t.periodic)
    {
        _usage -= 100;
    }

    else
    {
        --_usage;
    }

    if ((_cap == capabilities::dynamic || _cap == capabilities::periodic_capable) && _list.size() == 1 && _list.top()->periodic)
    {
        _is_periodic = true;
        _periodic(_list.top()->time_point - _now);
    }

    if (!_is_periodic && (_cap == capabilities::dynamic || _cap == capabilities::one_shot_capable) && _list.size())
    {
        if (_list.top()->time_point > t.time_point)
        {
            _one_shot(_list.top()->time_point - _now);
        }
    }
}

void processor::real_timer::_handle(processor::idt::isr_context isrc)
{
    LOCK(_lock);

    if (unlikely(_cap == capabilities::fixed_frequency))
    {
        _now += _minimal_tick;

        if (unlikely(!_list.size()))
        {
            return;
        }
    }

    else
    {
        if (unlikely(!_list.size()))
        {
            PANIC("Unexpected timer interrupt; timer list is empty.");
        }

        _now += _list.top()->time_point;
    }

    if (_is_periodic && _list.top()->time_point <= _now)
    {
        _list.top()->handler(isrc, _list.top()->handler_parameter);
        _list.update([](timer_description &){ return true; }, [](timer_description & desc){ desc.time_point += desc.period; });

        return;
    }

    while (_list.top()->time_point <= _now)
    {
        timer_description desc = _list.pop();
        desc.handler(isrc, desc.handler_parameter);

        if (desc.periodic)
        {
            desc.time_point += desc.period;
            _list.insert(desc);
        }

        else
        {
            --_usage;
        }
    }

    if (_list.size() == 0)
    {
        return;
    }

    if ((_cap == capabilities::dynamic || _cap == capabilities::periodic_capable) && _list.size() == 1 && _list.top()->periodic)
    {
        _is_periodic = true;
        _periodic(_list.top()->time_point - _now);

        return;
    }

    switch (_cap)
    {
        case capabilities::periodic_capable:
            _periodic(_list.top()->time_point - _now);
            break;

        case capabilities::dynamic:
            if (_is_periodic)
            {
                _periodic(_list.top()->period);

                break;
            }

        case capabilities::one_shot_capable:
            _one_shot(_list.top()->time_point - _now);

        default:
            ;
    };
}

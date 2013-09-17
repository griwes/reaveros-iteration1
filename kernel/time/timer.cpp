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

#include <time/timer.h>
#include <processor/idt.h>
#include <screen/screen.h>
#include <processor/core.h>

namespace
{
    std::atomic<uint64_t> _id = { 0 };

    time::timer * _hp_timer = nullptr;
}

uint64_t time::allocate_timer_event_id()
{
    return _id.fetch_add(1);
}

void time::high_precision_timer(time::timer * t)
{
    _hp_timer = t;
}

time::timer * time::high_precision_timer()
{
    return _hp_timer;
}

time::timer * time::preemption_timer()
{
    return &processor::get_current_core()->preemption_timer();
}

time::real_timer::real_timer(capabilities caps, uint64_t minimal_tick, uint64_t maximal_tick) : _cap{ caps },
    _is_periodic{}, _usage{}, _minimal_tick{ minimal_tick }, _maximal_tick{ maximal_tick }, _now{}
{
}

time::timer_event_handle time::real_timer::one_shot(uint64_t time, time::timer_handler handler, uint64_t context)
{
    INTL();
    LOCK(_lock);

    _update_now();

    timer_description desc;

    desc.id = time::allocate_timer_event_id();
    desc.handler = handler;
    desc.handler_parameter = context;
    desc.time_point = _now + time;

    _list.insert(desc);

    ++_usage;

    if (_is_periodic)
    {
        _is_periodic = false;
    }

    time = _list.top()->time_point - _now;
    time = time > _minimal_tick ? time : _minimal_tick;
    time = time < _maximal_tick ? time : _maximal_tick;

    if (_cap == capabilities::dynamic || _cap == capabilities::one_shot_capable)
    {
        _one_shot(time);
    }

    else if (_cap == capabilities::periodic_capable)
    {
        _periodic(time);
    }

    return { this, desc.id };
}

time::timer_event_handle time::real_timer::periodic(uint64_t period, time::timer_handler handler, uint64_t context)
{
    INTL();
    LOCK(_lock);

    _update_now();

    timer_description desc;

    desc.id = time::allocate_timer_event_id();
    desc.handler = handler;
    desc.handler_parameter = context;
    desc.periodic = true;
    desc.period = period;
    desc.time_point = _now + period;

    _list.insert(desc);

    _usage += 100;

    uint64_t time = _list.top()->time_point - _now;
    time = time > _minimal_tick ? time : _minimal_tick;
    time = time < _maximal_tick ? time : _maximal_tick;

    if (_list.size() == 1 && (_cap == capabilities::dynamic || _cap == capabilities::periodic_capable))
    {
        _one_shot(time);
    }

    else
    {
        _is_periodic = false;
        _one_shot(time);
    }

    return { this, desc.id };
}

void time::real_timer::cancel(uint64_t id)
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

    if (!_list.size())
    {
        _stop();
    }

    _update_now();

    uint64_t time = _list.top()->time_point - _now;
    time = time > _minimal_tick ? time : _minimal_tick;

    if ((_cap == capabilities::dynamic || _cap == capabilities::periodic_capable) && _list.size() == 1 && _list.top()->periodic)
    {
        _is_periodic = true;
        _periodic(time);
    }

    if (!_is_periodic && (_cap == capabilities::dynamic || _cap == capabilities::one_shot_capable) && _list.size())
    {
        if (_list.top()->time_point > t.time_point)
        {
            _one_shot(time);
        }
    }
}

void time::real_timer::_handle(processor::idt::isr_context & isrc)
{
    LOCK(_lock);

    if (unlikely(!_list.size()))
    {
        return;
    }

    if (unlikely(_cap == capabilities::fixed_frequency))
    {
        _now += _minimal_tick;
    }

    else
    {
        uint64_t tick = _list.top()->time_point - _now;

        if (tick > _maximal_tick)
        {
            tick = _maximal_tick;
        }

        else if (tick < _minimal_tick)
        {
            tick = _minimal_tick;
        }

        _now += tick;
    }

    if (_is_periodic)
    {
        while (_list.size() && _list.top()->time_point <= _now)
        {
            _list.top()->handler(isrc, _list.top()->handler_parameter);
            _list.update([=](timer_description & ref){ return true; }, [](timer_description & desc){ desc.time_point += desc.period; });
        }

        return;
    }

    auto scheduled = _schedule_next();

    while (_list.size() && _list.top()->time_point <= _now)
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

    if (_list.size() && (!scheduled || scheduled != _list.top()))
    {
        _schedule_next();
    }
}

void time::real_timer::_stop()
{
    if (_cap == capabilities::dynamic || _cap == capabilities::one_shot_capable)
    {
        _one_shot(_minimal_tick);
    }
}

const time::timer_description * time::real_timer::_schedule_next()
{
    if (!_list.size())
    {
        return nullptr;
    }

    auto next = _list.top();

    while (next->time_point <= _now && next->next)
    {
        next = next->next;
    }

    if (next != nullptr)
    {
        if (!_is_periodic && next->next == nullptr && next->periodic && (_cap == capabilities::dynamic || _cap
            == capabilities::periodic_capable))
        {
            _is_periodic = true;
            _periodic(next->period);
        }

        uint64_t time = next->time_point - _now;
        time = time > _minimal_tick ? time : _minimal_tick;
        time = time < _maximal_tick ? time : _maximal_tick;

        switch (_cap)
        {
            case capabilities::periodic_capable:
                _periodic(time);
                break;

            case capabilities::dynamic:
                if (_is_periodic)
                {
                    _periodic(next->period > _minimal_tick ? next->period : _minimal_tick);

                    break;
                }

            case capabilities::one_shot_capable:
                _one_shot(time);

            default:
                ;
        };
    }

    return next;
}

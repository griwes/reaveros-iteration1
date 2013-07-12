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

#include <time/lapic.h>
#include <processor/lapic.h>
#include <processor/idt.h>

namespace
{
    time::lapic::timer _timer;
}

void time::lapic::initialize()
{
    new (&_timer) time::lapic::timer{};
}

time::lapic::timer::timer() : real_timer{ capabilities::dynamic, 0, 0 }, _period{ 0 }, _lapic{ processor::get_lapic() }
{
    _lapic->divisor(1);
    _lapic->initial_count(~(uint32_t)0);
    get_high_precision_timer()->one_shot(1_ms, [](processor::idt::isr_context, uint64_t){}, 0);

    HLT;

    uint64_t ticks = ~(uint32_t)0 - _lapic->current_count();
    _stop();

    _period = (1_ms * 1000000) / ticks;

    _maximal_tick = (~(uint32_t)0 / 1000000) * _period * 128;
    _minimal_tick = _period / 1000000;

    screen::debug("\nLAPIC timer tick period: ", _period, "fs, ticks in 1ms: ", ticks);
}

void time::lapic::timer::_lapic_handler(processor::idt::isr_context isrc, uint64_t context)
{
    ((time::lapic::timer *)context)->_handle(isrc);

    static uint8_t i = 0;

    if (++i == 0)
    {
        ((time::lapic::timer *)context)->_update_now();
    }
}

void time::lapic::timer::_one_shot(uint64_t time)
{
    uint8_t divisor = 1;
    while (((time * 1000000) / (divisor * _period)) > ~(uint32_t)0 && divisor != 128)
    {
        divisor *= 2;
    }

    time = (time * 1000000) / (divisor * _period);
    if (time > ~(uint32_t)0)
    {
        time = ~(uint32_t)0;
    }

    _lapic->divisor(divisor);
    _lapic->set_timer(false);
    _lapic->initial_count(time);
}

void time::lapic::timer::_periodic(uint64_t period)
{
    uint8_t divisor = 1;
    while (((period * 1000000) / (divisor * _period)) > ~(uint32_t)0 && divisor != 128)
    {
        divisor *= 2;
    }

    period = (period * 1000000) / (divisor * _period);
    if (period > ~(uint32_t)0)
    {
        period = ~(uint32_t)0;
    }

    _lapic->divisor(divisor);
    _lapic->set_timer(true);
    _lapic->initial_count(period);
}

void time::lapic::timer::_update_now()
{
    if (!_list.size())
    {
        return;
    }

    uint8_t divisor = _lapic->divisor();

    uint64_t time = _list.top()->time_point - _now;
    time = (time * 1000000) / (divisor * _period);
    if (time > ~(uint32_t)0)
    {
        time = ~(uint32_t)0;
    }

    time -= _lapic->current_count();;
    time = (time * divisor * _period) / 1000000;

    _now += time;
}

void time::lapic::timer::_stop()
{
    _lapic->initial_count(0);
}

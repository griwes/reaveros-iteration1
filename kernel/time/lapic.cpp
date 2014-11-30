/**
 * Reaver Project OS, Rose License
 *
 * Copyright © 2013-2014 Michał "Griwes" Dominiak
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
 **/

#include <time/lapic.h>
#include <processor/lapic.h>
#include <processor/idt.h>
#include <processor/handlers.h>
#include <processor/core.h>

namespace
{
    time::lapic::timer * _bsp_timer = nullptr;

    bool _fired = false;
}

void time::lapic::initialize()
{
    processor::get_current_core()->initialize_preemption_timer();
    _bsp_timer = &processor::get_current_core()->preemption_timer();
}

void time::lapic::ap_initialize()
{
    processor::get_current_core()->initialize_preemption_timer(*_bsp_timer);
}

time::lapic::timer::timer() : real_timer{ capabilities::dynamic, 0, 0 }, _period{ 0 }, _lapic{ processor::get_lapic() }
{
    _lapic->divisor(1);
    _lapic->initial_count(~(uint32_t)0);
    high_precision_timer()->one_shot(1_ms, [](uint64_t)
    {
        _fired = true;
    }, 0);

    while (!_fired)
    {
        HLT;
    }

    uint64_t ticks = ~(uint32_t)0 - _lapic->current_count();
    _stop();

    _period = (1_ms * 1000000) / ticks;

    _maximal_tick = (~(uint32_t)0 / 1000000) * _period * 128;
    _minimal_tick = _period / 1000000;

    screen::debug("\nLAPIC timer tick period: ", _period, "fs, ticks in 1ms: ", ticks);

    processor::register_handler(_lapic->_timer_irq, _lapic_handler);
}

time::lapic::timer::timer(const time::lapic::timer & rhs) : real_timer{ capabilities::dynamic, rhs._minimal_tick,
    rhs._maximal_tick }, _period{ rhs._period }, _lapic{ processor::get_lapic() }
{
}

void time::lapic::timer::_lapic_handler(processor::isr_context & isrc, uint64_t context)
{
    static_cast<timer *>(preemption_timer())->_handle();

    static uint8_t i = 0;

    if (!(++i % 64))
    {
        static_cast<timer *>(preemption_timer())->_update_now();
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
    _lapic->initial_count(time);
    _lapic->set_timer(false);
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
    _lapic->initial_count(period);
    _lapic->set_timer(true);
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

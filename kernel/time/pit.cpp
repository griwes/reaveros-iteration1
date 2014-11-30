/**
 * Reaver Project OS, Rose License
 *
 * Copyright © 2013 Michał "Griwes" Dominiak
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

#include <time/pit.h>
#include <processor/handlers.h>
#include <screen/screen.h>

namespace
{
    utils::lazy<time::pit::timer> _pit;
    bool _ready = false;
}

void time::pit::timer::_pit_handler(processor::isr_context & isr, uint64_t context)
{
    ((time::pit::timer *)context)->_handle();
}

void time::pit::initialize()
{
    _pit.initialize();
    _ready = true;

    time::high_precision_timer(&*_pit);
}

bool time::pit::ready()
{
    return _ready;
}

time::pit::timer::timer() : real_timer{ capabilities::dynamic, 200_us, 1_s / 19 }, _int_vector{}
{
    _int_vector = processor::allocate_isr(0);
    processor::register_handler(_int_vector, _pit_handler, (uint64_t)this);

    _one_shot(200_us);
    processor::set_isa_irq_int_vector(0, _int_vector);

    STI;
    HLT;
    CLI;

    screen::debug("\nPIT handler installed successfully");
}

void time::pit::timer::_one_shot(uint64_t time)
{
    if (time >= 1_s / 19)
    {
        time = 1_s / 19;
    }

    uint64_t hz = 1000000000ull / time;
    uint16_t divisor = 1193180 / hz;

    outb(0x43, 0x30);

    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

void time::pit::timer::_periodic(uint64_t period)
{
    if (period >= 1_s / 19)
    {
        period = 1_s / 19;
    }

    uint64_t hz = 1000000000ull / period;
    uint16_t divisor = 1193180 / hz;

    outb(0x43, 0x34);
    outb(0x40, (uint8_t)(divisor & 0xFF));
    outb(0x40, (uint8_t)((divisor >> 8) & 0xFF));
}

void time::pit::timer::_update_now()
{
    if (_list.size())
    {
        outb(0x43, 0);
        uint16_t count = inb(0x40);
        count |= inb(0x40) << 8;

        uint64_t time = _list.top()->time_point - _now;

        if (unlikely(time < 200_us))
        {
            time = 2_ms;
        }

        else if (unlikely(time > 1_s / 19))
        {
            time = 1_s / 19;
        }

        uint64_t hz = 1000000000ull / time;
        uint16_t divisor = 1193180 / hz;
        count = divisor - count;

        _now += 838_ns * count;
    }
}

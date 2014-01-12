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

#include <time/real.h>
#include <time/timer.h>
#include <processor/idt.h>
#include <time/hpet.h>

namespace
{
    utils::spinlock _lock;
    time::point _boot_time = { 0, 0 };
    time::point _time = { 0, 0 };
    uint64_t _hpet_last_ns = 0;

    bool _rtc_in_update()
    {
        outb(0x70, 0x0a);
        return inb(0x71) & 0x80;
    }

    uint8_t _register(uint8_t reg)
    {
        outb(0x70, reg);
        return inb(0x71);
    }

    constexpr uint64_t _current_century = 20;

    void _update(uint64_t)
    {
        LOCK(_lock);

        _time.nanoseconds += 2_ms;

        if (_time.nanoseconds > 1_s)
        {
            _time.nanoseconds -= 1_s;
            ++_time.seconds;
        }
    }

    uint64_t _epoch_year = 2001;
    uint8_t _epoch_month = 1;
    uint8_t _epoch_day = 1;
}

void time::real::initialize()
{
    uint8_t second, last_second, minute, last_minute, hour, last_hour, day, last_day, month, last_month;
    uint64_t year, last_year;
    uint8_t regb;

    while (_rtc_in_update())
    {
        asm volatile ("pause");
    }

    second = _register(0x0);
    minute = _register(0x2);
    hour = _register(0x4);
    day = _register(0x7);
    month = _register(0x8);
    year = _register(0x9);

    do
    {
        last_second = second;
        last_minute = minute;
        last_hour = hour;
        last_day = day;
        last_month = month;
        last_year = year;

        while (_rtc_in_update())
        {
            asm volatile ("pause");
        }

        second = _register(0x0);
        minute = _register(0x2);
        hour = _register(0x4);
        day = _register(0x7);
        month = _register(0x8);
        year = _register(0x9);
    } while (second != last_second || minute != last_minute || hour != last_hour || day != last_day || month != last_month
        || year != last_year);

    if (!hpet::ready())
    {
        high_precision_timer()->periodic(2_ms, _update, 0);
    }

    else
    {
        _hpet_last_ns = ((hpet::timer *)high_precision_timer())->now();
    }

    STI;

    regb = _register(0xb);

    if (!(regb & 0x04))
    {
        second = (second & 0x0F) + ((second / 16) * 10);
        minute = (minute & 0x0F) + ((minute / 16) * 10);
        hour = ((hour & 0x0F) + (((hour & 0x70) / 16) * 10)) | (hour & 0x80);
        day = (day & 0x0F) + ((day / 16) * 10);
        month = (month & 0x0F) + ((month / 16) * 10);
        year = (year & 0x0F) + ((year / 16) * 10);
    }

    if (!(regb & 0x02) && (hour & 0x80))
    {
        hour = ((hour & 0x7F) + 12) % 24;
    }

    year += _current_century * 100;

    uint64_t years = year - _epoch_year;
    uint64_t days = years * 365 + years / 4 - years / 100 + years / 400;
    days += (month > 1) * 31 + (month > 2) * 28 + (month > 3) * 31 + (month > 4) * 30 + (month > 5) * 31 + (month > 6) * 30
        + (month > 7) * 31 + (month > 8) * 31 + (month > 9) * 30 + (month > 10) * 31 + (month > 11) * 30;
    days += day;
    uint64_t seconds = ((days * 24 + hour) * 60 + minute) * 60 + second;

    {
        INTL();
        LOCK(_lock);
        _time.seconds += seconds;
        _boot_time.seconds = seconds;
    }
}

time::point time::real::now()
{
    INTL();
    LOCK(_lock);

    if (hpet::ready())
    {
        uint64_t hpet_now = ((hpet::timer *)high_precision_timer())->now();
        uint64_t nanoseconds = hpet_now - _hpet_last_ns;
        _hpet_last_ns = hpet_now;
        _time.seconds += nanoseconds / 1000000000;
        _time.nanoseconds += nanoseconds % 1000000000;

        if (_time.nanoseconds >= 1_s)
        {
            _time.nanoseconds -= 1_s;
            ++_time.seconds;
        }
    }

    return _time;
}

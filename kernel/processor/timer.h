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

namespace processor
{
    uint64_t allocate_timer_event_id();

    class timer;

    using timer_handler = void (*)(timer *);

    struct timer_event_handle;

    class timer
    {
    public:
        virtual ~timer() {}

        virtual timer_event_handle one_shot(uint64_t, timer_handler) = 0;
        virtual timer_event_handle periodic(uint64_t, timer_handler) = 0;
        virtual void cancel(uint64_t) = 0;
    };

    struct timer_event_handle
    {
        timer * device;
        uint64_t id;

        void cancel()
        {
            device->cancel(id);
        }
    };

    void set_high_precision_timer(timer *);
    void set_scheduling_timer(timer *);

    timer * get_high_precision_timer();
    timer * get_scheduling_timer();
}

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

#pragma once

#include <utils/spinlock.h>
#include <utils/allocator.h>
#include <utils/priority_list.h>
#include <processor/context.h>

namespace time
{
    uint64_t allocate_timer_event_id();

    using timer_handler = void (*)(uint64_t);

    struct timer_event_handle;

    class timer
    {
    public:
        virtual ~timer() {}

        virtual timer_event_handle one_shot(uint64_t, timer_handler, uint64_t = 0) = 0;
        virtual timer_event_handle periodic(uint64_t, timer_handler, uint64_t = 0) = 0;
        virtual void cancel(uint64_t) = 0;
    };

    struct timer_description : public utils::chained<timer_description>
    {
        timer_description() : id{}, handler{}, handler_parameter{}, periodic{}, time_point{}, period{}
        {
        }

        uint64_t id;
        timer_handler handler;
        uint64_t handler_parameter;
        uint64_t periodic:1;
        uint64_t time_point;
        uint64_t period;
    };

    struct timer_description_comparator
    {
        bool operator()(const timer_description & lhs, const timer_description & rhs)
        {
            return lhs.time_point < rhs.time_point;
        }
    };

    class real_timer : public timer
    {
    public:
        enum class capabilities
        {
            periodic_capable,
            one_shot_capable,
            fixed_frequency,
            dynamic
        };

        real_timer(capabilities, uint64_t, uint64_t);

        virtual ~real_timer() {}

        virtual timer_event_handle one_shot(uint64_t, timer_handler, uint64_t = 0);
        virtual timer_event_handle periodic(uint64_t, timer_handler, uint64_t = 0);
        virtual void cancel(uint64_t);

        uint64_t usage()
        {
            return _usage;
        }

    protected:
        virtual void _one_shot(uint64_t) = 0;
        virtual void _periodic(uint64_t) = 0;
        virtual void _update_now() = 0;
        virtual void _stop();

        void _handle();
        const timer_description * _schedule_next();

        capabilities _cap;

        bool _is_periodic;
        uint64_t _usage;
        uint64_t _minimal_tick;
        uint64_t _maximal_tick;

        uint64_t _now;

        utils::recursive_spinlock _lock;
        utils::priority_list<timer_description, timer_description_comparator> _list;
    };

    struct timer_event_handle
    {
        timer * device;
        uint64_t id;

        void cancel()
        {
            if (device)
            {
                device->cancel(id);
            }

            device = nullptr;
        }
    };

    void high_precision_timer(timer *);
    timer * high_precision_timer();
    timer * preemption_timer();
}

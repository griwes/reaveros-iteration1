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

#include <processor/pit.h>
#include <processor/handlers.h>

namespace
{
    processor::pit::timer * _pit = nullptr;

}


void processor::_detail::_pit_handler(processor::idt::isr_context isr, uint64_t context)
{
    ((processor::pit::timer *)context)->_handle(isr);
}

void processor::pit::initialize()
{
    new (_pit) processor::pit::timer();

    processor::set_high_precision_timer(_pit);
}

bool processor::pit::ready()
{
    return _pit;
}

processor::pit::timer::timer() : _periodic{}, _usage{}, _int_vector{}, _active_timers{}, _free_descriptors{}
{
    _int_vector = allocate_isr(0);
    register_handler(_int_vector, _detail::_pit_handler, (uint64_t)this);
    set_isa_irq_int_vector(0, _int_vector);
}

processor::timer_event_handle processor::pit::timer::one_shot(uint64_t , processor::timer_handler , uint64_t )
{
    return {};
}

processor::timer_event_handle processor::pit::timer::periodic(uint64_t , processor::timer_handler , uint64_t )
{
    return {};
}

void processor::pit::timer::cancel(uint64_t )
{

}

void processor::pit::timer::_handle(processor::idt::isr_context isrc)
{
    LOCK(_lock);

    _active_timers->handler(isrc, _active_timers->handler_parameter);

    if (!_periodic)
    {
        auto first_free = _free_descriptors;
        _free_descriptors = _active_timers;
        _active_timers = _active_timers->next;
        _free_descriptors->next = first_free;
        _free_descriptors->prev = nullptr;

        if (_active_timers)
        {
            if (_active_timers->period < 200000)
            {
                _active_timers->next->period += 200000 - _active_timers->period;
                _active_timers->period = 200000;
            }

//            _one_shot(_active_timers->period);
        }
    }
}

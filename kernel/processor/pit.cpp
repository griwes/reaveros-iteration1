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
#include <screen/screen.h>

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

processor::pit::timer::timer() : real_timer{ true }, _int_vector{}
{
    _int_vector = allocate_isr(0);
    register_handler(_int_vector, _detail::_pit_handler, (uint64_t)this);
    set_isa_irq_int_vector(0, _int_vector);
}

void processor::pit::timer::_one_shot(uint64_t time)
{

}

void processor::pit::timer::_periodic(uint64_t period)
{

}

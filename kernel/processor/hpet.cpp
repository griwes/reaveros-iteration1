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

#include <processor/hpet.h>
#include <acpi/acpi.h>

namespace
{
    processor::hpet::timer * _timers;
    uint64_t _num_timers;
}

void processor::hpet::initialize()
{
    acpi::parse_hpet(_timers, _num_timers);
}

bool processor::hpet::ready()
{
    return _num_timers;
}

processor::hpet::timer::timer(uint8_t number, pci_vendor_t pci_vendor, uint64_t address, uint8_t counter_size,
    uint8_t comparators, uint16_t minimum_tick, uint8_t page_protection) : _number{ number }, _pci_vendor{ pci_vendor.vendor },
    _size{ (uint8_t)(32 + 32 * counter_size) }, _comparators{ comparators }, _minimum_tick{ minimum_tick },
    _page_protection{ page_protection }, _register{ address }, _active_timers{}, _free_descriptors{}
{

}

processor::timer_event_handle processor::hpet::timer::one_shot(uint64_t , processor::timer_handler )
{
    return {};
}

processor::timer_event_handle processor::hpet::timer::periodic(uint64_t , processor::timer_handler )
{
    return {};
}

void processor::hpet::timer::cancel(uint64_t )
{

}

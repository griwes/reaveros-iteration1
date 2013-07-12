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

#include <time/hpet.h>
#include <acpi/acpi.h>
#include <screen/screen.h>
#include <processor/handlers.h>

namespace
{
    time::hpet::timer * _timers = nullptr;
    uint64_t _num_timers = 0;

    uint32_t _used_interrupts = 0;

    enum _hpet_registers
    {
        _general_capabilities = 0x0,
        _general_configuration = 0x10,
        _interrupt_status = 0x20,
        _main_counter = 0xF0
    };

    uint64_t _timer_configuration(uint8_t i)
    {
        return 0x100 + 0x20 * i;
    }

    uint64_t _timer_comparator(uint8_t i)
    {
        return 0x108 + 0x20 * i;
    }

    uint64_t _fsb_route(uint8_t i)
    {
        return 0x110 + 0x20 * i;
    }
}

void time::hpet::initialize()
{
    _used_interrupts |= 1;
    _used_interrupts |= 1 << processor::translate_isa(0);
    _used_interrupts |= 1 << processor::translate_isa(8);

    acpi::parse_hpet(_timers, _num_timers);

    if (_num_timers)
    {
        time::set_high_precision_timer(_timers);
    }
}

bool time::hpet::ready()
{
    return _num_timers;
}

time::hpet::timer::timer(uint8_t number, pci_vendor_t pci_vendor, uint64_t address, uint8_t counter_size,
    uint8_t comparators, uint16_t minimal_tick, uint8_t page_protection) : _number{ number }, _size{ (uint8_t)(32 + 32 * counter_size) },
    _comparator_count{ comparators }, _page_protection{ page_protection }, _pci_vendor{ pci_vendor.vendor }, _minimal_tick{
    minimal_tick }, _register{ address }
{
    _register(_general_configuration, _register(_general_configuration) | 3);

    _period = _register(_general_capabilities) >> 32;
    _frequency = 1000000000000000ull / _period;

    if (_size == 32)
    {
        _maximal_tick = (~(uint32_t)0 / 1000000) * _period;

        if (!_maximal_tick)
        {
            _maximal_tick = -1;
        }
    }

    else
    {
        _maximal_tick = (~0ull / 1000000) * _period;
    }

    screen::debug("\nDetected HPET counter period in fs: ", _period);
    screen::debug("\nDetected HPET counter frequency: ", _frequency);

    for (uint8_t i = 0; i < _comparator_count; ++i)
    {
        new (_comparators + i) comparator{ this, i };
    }

    _register(_main_counter, 0);

    _comparators[0].one_shot(1_us, [](processor::idt::isr_context, uint64_t){});
    STI;
    HLT;
    CLI;
}

time::timer_event_handle time::hpet::timer::one_shot(uint64_t time, time::timer_handler handler, uint64_t param)
{
    uint64_t min = _comparators[0].usage();
    uint64_t min_idx = 0;

    for (uint8_t i = 1; i < _comparator_count; ++i)
    {
        if (_comparators[i].valid() && _comparators[i].usage() < min)
        {
            min_idx = i;
            min = _comparators[i].usage();
        }
    }

    return _comparators[min_idx].one_shot(time, handler, param);
}

time::timer_event_handle time::hpet::timer::periodic(uint64_t period, time::timer_handler handler, uint64_t param)
{
    uint64_t min = _comparators[0].usage();
    uint64_t min_idx = 0;

    for (uint8_t i = 1; i < _comparator_count; ++i)
    {
        if (_comparators[i].valid() && _comparators[i].usage() < min)
        {
            min_idx = i;
            min = _comparators[i].usage();
        }
    }

    return _comparators[min_idx].periodic(period, handler, param);
}

void time::hpet::timer::cancel(uint64_t)
{
    NEVER;
}

uint64_t time::hpet::timer::now()
{
    return (_register(_main_counter) * _period) / 1000000;
}

void time::hpet::comparator::_hpet_handler(processor::idt::isr_context isrc, uint64_t context)
{
    ((time::hpet::comparator *)context)->_handle(isrc);

    static uint8_t i = 0;

    if (++i == 0)
    {
        ((time::hpet::comparator *)context)->_update_now();
    }
}

time::hpet::comparator::comparator() : real_timer{ capabilities::dynamic, 0, 0 }, _parent{}
{
}

time::hpet::comparator::comparator(time::hpet::timer * parent, uint8_t index) : real_timer{ capabilities::dynamic,
    parent->_minimal_tick, parent->_maximal_tick }, _parent{ parent }, _index{ index }, _input{}
{
    if (!(_parent->_register(_timer_configuration(_index)) & (1 << 4)))
    {
        _cap = capabilities::one_shot_capable;
    }

    if (_index < 2)
    {
        _int_vector = processor::allocate_isr(0);
        processor::register_handler(_int_vector, _hpet_handler, (uint64_t)this);
        processor::set_isa_irq_int_vector(_index * 8, _int_vector);

        screen::debug("\nInstalled interrupt for HPET comparator #", _index, " at IOAPIC input #",
            processor::translate_isa(_index * 8), " routed to vector ", _int_vector);

        return;
    }

    if (_parent->_register(_timer_configuration(_index)) & (1 << 15))
    {
        screen::debug("\nHPET comparator #", _index, " is FSB capable; better implement FSB interrupt routing.");

        // TODO
    }

    uint32_t possible_routes = _parent->_register(_timer_configuration(_index)) >> 32;
    screen::debug("\nPossible IOAPIC inputs for HPET comparator #", _index, ": ");

    for (uint8_t i = 0; i < 32; ++i)
    {
        if (possible_routes & (1 << i))
        {
            screen::debug(i, " ");
        }
    }

    for (uint8_t i = 0; i < 32 && i < processor::max_ioapic_input() && possible_routes != ~(uint32_t)0; ++i)
    {
        if ((possible_routes & (1 << i)) && !(_used_interrupts & (1 << i)))
        {
            _int_vector = processor::allocate_isr(0);
            processor::register_handler(_int_vector, _hpet_handler, (uint64_t)this);
            processor::set_isa_irq_int_vector(i, _int_vector);

            _used_interrupts |= 1 << i;
            _input = i;

            screen::debug("\nInstalled interrupt for HPET comparator #", _index, " at IOAPIC input #", i, " routed to vector ",
                _int_vector);

            return;
        }
    }

    _parent = nullptr;
    screen::debug("\nCouldn't find interrupt routing for HPET comparator #", _index, "; disabling");
}

void time::hpet::comparator::_one_shot(uint64_t time)
{
    _parent->_register(_timer_configuration(_index), ((_input & 31) << 9) | (1 << 2));
    _parent->_register(_timer_comparator(_index), ((_now + time) * 1000000) / _parent->_period);
}

void time::hpet::comparator::_periodic(uint64_t period)
{
    _parent->_register(_timer_configuration(_index), ((_input & 31) << 9) | (1 << 2) | (1 << 3) | (1 << 6));
    _parent->_register(_timer_comparator(_index), ((_now + period) * 1000000) / _parent->_period);
    _parent->_register(_timer_comparator(_index), (period * 1000000) / _parent->_period);
}

void time::hpet::comparator::_update_now()
{
    _now = (_parent->_register(_main_counter) * _parent->_period) / 1000000;
}

void time::hpet::comparator::_stop()
{
    _parent->_register(_timer_configuration(_index), _parent->_register(_timer_configuration(_index)) & ~(1ull << 2));
}

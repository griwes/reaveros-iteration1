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

#include <processor/x2apic.h>
#include <screen/screen.h>
#include <processor/handlers.h>

namespace
{
    enum x2apic_registers
    {
        _apic_id = 0x802,
        _apic_version = 0x803,
        _task_priority = 0x808,
        _processor_priority = 0x80A,
        _eoi = 0x80B,
        _logical_destination = 0x80D,
        _spurious_interrupt_vector = 0x80F,
        _in_service_0 = 0x810,
        _in_service_32 = 0x811,
        _in_service_64 = 0x812,
        _in_service_96 = 0x813,
        _in_service_128 = 0x814,
        _in_service_160 = 0x815,
        _in_service_192 = 0x816,
        _in_service_224 = 0x817,
        _trigger_mode_0 = 0x818,
        _trigger_mode_32 = 0x819,
        _trigger_mode_64 = 0x81A,
        _trigger_mode_96 = 0x81B,
        _trigger_mode_128 = 0x81C,
        _trigger_mode_160 = 0x81D,
        _trigger_mode_192 = 0x81E,
        _trigger_mode_224 = 0x81F,
        _interrupt_request_0 = 0x820,
        _interrupt_request_32 = 0x821,
        _interrupt_request_64 = 0x822,
        _interrupt_request_96 = 0x823,
        _interrupt_request_128 = 0x824,
        _interrupt_request_160 = 0x825,
        _interrupt_request_192 = 0x826,
        _interrupt_request_224 = 0x827,
        _error_status = 0x828,
        _lvt_cmci = 0x82F,
        _interrupt_command = 0x830,
        _lvt_timer = 0x832,
        _lvt_thermal_sensor = 0x833,
        _lvt_performance_monitor = 0x834,
        _lvt_lint0 = 0x835,
        _lvt_lint1 = 0x836,
        _lvt_error = 0x837,
        _initial_count = 0x838,
        _current_count = 0x839,
        _divide_configuration = 0x83E,
        _self_ipi = 0x83F
    };

    uint64_t _apic_base_msr = 0x01B;
}

processor::x2apic::x2apic()
{
    wrmsr(_apic_base_msr, rdmsr(_apic_base_msr) | (1 << 10));

    if (((_register(_apic_version) >> 16) & 0xFF) == 6)
    {
        _register(_lvt_cmci, 0x10000);
    }

    _register(_lvt_error, 0x10000);
    _register(_lvt_lint0, 0x10000);
    _register(_lvt_lint1, 0x10000);
    _register(_lvt_performance_monitor, 0x10000);
    _register(_lvt_thermal_sensor, 0x10000);
    _register(_lvt_timer, 0x10000);

    _timer_irq = allocate_isr(0);
    _spurious = allocate_isr(13);

    _register(_spurious_interrupt_vector, _spurious | 0x100);

    screen::debug("\nInitialized x2APIC. APIC version: ", _register(_apic_version) & 0xFF, ", number of LVTs: ",
        ((_register(_apic_version) >> 16 ) & 0xFF) + 1);
}

void processor::x2apic::eoi(uint8_t v)
{
    if (v != _spurious)
    {
        _register(_eoi, 0);
    }
}

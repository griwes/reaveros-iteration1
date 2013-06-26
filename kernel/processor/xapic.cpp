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

#include <processor/xapic.h>

namespace
{
    enum xapic_registers
    {
        _apic_id = 0x20,
        _apic_version = 0x30,
        _task_priority = 0x80,
        _arbitrary_priority = 0x90,
        _processor_priority = 0xA0,
        _eoi = 0xB0,
        _remote_read = 0xC0,
        _logical_destination = 0xD0,
        _destination_format = 0xE0,
        _spurious_interrupt_vector = 0xF0,
        _in_service_0 = 0x100,
        _in_service_32 = 0x110,
        _in_service_64 = 0x120,
        _in_service_96 = 0x130,
        _in_service_128 = 0x140,
        _in_service_160 = 0x150,
        _in_service_192 = 0x160,
        _in_service_224 = 0x170,
        _trigger_mode_0 = 0x180,
        _trigger_mode_32 = 0x190,
        _trigger_mode_64 = 0x1A0,
        _trigger_mode_96 = 0x1B0,
        _trigger_mode_128 = 0x1C0,
        _trigger_mode_160 = 0x1D0,
        _trigger_mode_192 = 0x1E0,
        _trigger_mode_224 = 0x1F0,
        _interrupt_request_0 = 0x200,
        _interrupt_request_32 = 0x210,
        _interrupt_request_64 = 0x220,
        _interrupt_request_96 = 0x230,
        _interrupt_request_128 = 0x240,
        _interrupt_request_160 = 0x250,
        _interrupt_request_192 = 0x260,
        _interrupt_request_224 = 0x270,
        _error_status = 0x280,
        _lvt_cmci = 0x2F0,
        _interrupt_command_0 = 0x300,
        _interrupt_command_32 = 0x310,
        _lvt_timer = 0x320,
        _lvt_thermal_sensor = 0x330,
        _lvt_performance_monitor = 0x340,
        _lvt_lint0 = 0x350,
        _lvt_lint1 = 0x360,
        _lvt_error = 0x370,
        _initial_count = 0x380,
        _current_count = 0x390,
        _divide_configuration = 0x3E0
    };
}
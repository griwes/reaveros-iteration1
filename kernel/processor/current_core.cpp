/**
 * Reaver Project OS, Rose License
 *
 * Copyright (C) 2011-2013 Reaver Project Team:
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

#include <processor/current_core.h>
#include <processor/handlers.h>
#include <memory/vm.h>
#include <screen/screen.h>
#include <processor/pit.h>
#include <processor/processor.h>

namespace
{
    enum lapic_registers
    {
        apic_id = 0x20,
        apic_version = 0x30,
        task_priority = 0x80,
        arbitrary_priority = 0x90,
        processor_priority = 0xA0,
        eoi = 0xB0,
        remote_read = 0xC0,
        logical_destination = 0xD0,
        destination_format = 0xE0,
        spurious_interrupt_vector = 0xF0,
        in_service_0 = 0x100,
        in_service_32 = 0x110,
        in_service_64 = 0x120,
        in_service_96 = 0x130,
        in_service_128 = 0x140,
        in_service_160 = 0x150,
        in_service_192 = 0x160,
        in_service_224 = 0x170,
        trigger_mode_0 = 0x180,
        trigger_mode_32 = 0x190,
        trigger_mode_64 = 0x1A0,
        trigger_mode_96 = 0x1B0,
        trigger_mode_128 = 0x1C0,
        trigger_mode_160 = 0x1D0,
        trigger_mode_192 = 0x1E0,
        trigger_mode_224 = 0x1F0,
        interrupt_request_0 = 0x200,
        interrupt_request_32 = 0x210,
        interrupt_request_64 = 0x220,
        interrupt_request_96 = 0x230,
        interrupt_request_128 = 0x240,
        interrupt_request_160 = 0x250,
        interrupt_request_192 = 0x260,
        interrupt_request_224 = 0x270,
        error_status = 0x280,
        lvt_cmci = 0x2F0,
        interrupt_command_0 = 0x300,
        interrupt_command_32 = 0x310,
        lvt_timer = 0x320,
        lvt_thermal_sensor = 0x330,
        lvt_performance_monitor = 0x340,
        lvt_lint0 = 0x350,
        lvt_lint1 = 0x360,
        lvt_error = 0x370,
        initial_count = 0x380,
        current_count = 0x390,
        divide_configuration = 0x3E0
    };

    inline void _write_register(uint32_t reg, uint32_t val)
    {
        *(volatile uint32_t *)(memory::vm::local_apic_address + reg) = val;
    }

    inline uint32_t _read_register(uint32_t reg)
    {
        return *(volatile uint32_t *)(memory::vm::local_apic_address + reg);
    }

    void _spurious(processor::idt::irq_context)
    {
    }

    bool _invoked = false;
    uint64_t _ticks_per_second = 0;

    uint8_t _spurious_vector = 0;
    uint8_t _timer_vector = 0;

    uint8_t _panic_vector = 0;

    void _calibrate_local_timer(processor::idt::irq_context)
    {
        _invoked = true;
    }

    void _lapic_timer(processor::idt::irq_context ctx)
    {
    }

    void _panic_core(processor::idt::irq_context)
    {
        screen::console.release();
        asm volatile ("cli; hlt");
    }
}

void processor::current_core::eoi(uint8_t vec_number)
{
    if (vec_number == (_read_register(spurious_interrupt_vector) & 0xFF))
    {
        return;
    }

    _write_register(::eoi, 0);
}

void processor::current_core::initialize()
{
    _write_register(destination_format, _read_register(destination_format) & 0xF0000000);
    _write_register(logical_destination, 0xFF000000);

    // TODO: add CPUID checks and enable model-dependant LVT disable
    _write_register(lvt_cmci, 0x10000);                     // disable LVTs
    _write_register(lvt_error, 0x10000);
    _write_register(lvt_lint0, 0x10000);
    _write_register(lvt_lint1, 0x10000);
//    _write_register(lvt_performance_monitor, 0x10000);
//    _write_register(lvt_thermal_sensor, 0x10000);
    _write_register(lvt_timer, 0x10000);

    _write_register(task_priority, 0);

    uint32_t a, b;
    rdmsr(0x1B, a, b);
    wrmsr(0x1B, a | (1 << 11), b);

    if (_ticks_per_second)
    {
        _write_register(spurious_interrupt_vector, _spurious_vector | 0x100);
        _write_register(lvt_timer, _timer_vector | (1 << 17));
        _write_register(divide_configuration, 3);

        return;
    }

    _spurious_vector = interrupts::allocate(_spurious);

    _write_register(spurious_interrupt_vector, _spurious_vector | 0x100);

    _timer_vector = interrupts::allocate(_calibrate_local_timer, 2);

    _write_register(lvt_timer, _timer_vector | (1 << 17));
    _write_register(divide_configuration, 3);

    _panic_vector = interrupts::allocate(_panic_core, 2);

    // TODO: HPET

/*    if (hpet::present())
    {
        hpet::register_interrupt(_calibrate_local_timer);
        hpet::interrupt(1000);
    }

    else
    {*/
        pit::register_callback(_calibrate_local_timer);
        pit::interrupt(100);
    /*}*/

    _write_register(initial_count, 0xFFFFFFFF);

    while (!_invoked)
    {
        asm volatile ("pause");
    }

    _ticks_per_second = (0xFFFFFFFF - _read_register(current_count)) * 16 * 100;
    _write_register(initial_count, 0);

    screen::debug("\nLAPIC tics per second (estimated): ", _ticks_per_second);

    interrupts::remove_handler(_timer_vector);
    interrupts::set_handler(_timer_vector, _lapic_timer);
}

void processor::current_core::sleep(uint64_t nanoseconds)
{
    _write_register(divide_configuration, 3);
    _write_register(initial_count, (_ticks_per_second / 16) / (1000000000 / nanoseconds));

    asm volatile ("hlt");
}

void processor::current_core::ipi(uint64_t apic_id, processor::current_core::ipis type, uint8_t vector)
{
    _write_register(interrupt_command_32, apic_id << 24);

    switch (type)
    {
        case ipis::init:
        {
            _write_register(interrupt_command_0, 5 << 8);

            break;
        }

        case ipis::sipi:
        {
            _write_register(interrupt_command_0, (6 << 8) | (vector));

            break;
        }

        default:
        {
            break;
        }
    }
}

void processor::current_core::broadcast(processor::current_core::broadcast_types type, processor::current_core::ipis ipi, uint8_t vector)
{
    _write_register(interrupt_command_32, 0xFF << 24);

    switch (ipi)
    {
        case ipis::panic:
        {
            _write_register(interrupt_command_0, ((uint8_t)type << 18) | _panic_vector);

            break;
        }

        default:
        {
            break;
        }
    }
}


uint32_t processor::current_core::id()
{
    if (_ticks_per_second)
    {
        return _read_register(apic_id) >> 24;
    }

    return 0;
}

void processor::current_core::stop_timer()
{
    _write_register(initial_count, 0);
}

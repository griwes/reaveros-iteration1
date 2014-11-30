/**
 * Reaver Project OS, Rose License
 *
 * Copyright © 2013-2014 Michał "Griwes" Dominiak
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

#include <processor/processor.h>
#include <memory/memory.h>
#include <processor/interrupt_entry.h>
#include <processor/gdt.h>
#include <processor/idt.h>
#include <acpi/acpi.h>
#include <processor/ioapic.h>
#include <time/hpet.h>
#include <processor/handlers.h>
#include <processor/lapic.h>
#include <time/pit.h>
#include <time/real.h>
#include <processor/smp.h>
#include <processor/core.h>
#include <memory/vm.h>
#include <processor/ipi.h>
#include <processor/syscalls.h>

namespace
{
    virt_addr_t _lapic_base;

    processor::core * _cores;
    processor::ioapic * _ioapics;

    uint64_t _num_cores;
    uint64_t _num_ioapics;

    uint8_t _max_ioapic_input = 0;

    processor::interrupt_entry _sources[128] = {};

    std::atomic<bool> _ready{ false };

    uint64_t _bsp = 0;
}

uint64_t processor::bsp()
{
    return _bsp;
}

bool processor::ready()
{
    return _ready;
}

virt_addr_t processor::lapic_base()
{
    return _lapic_base;
}

uint8_t processor::translate_isa(uint8_t irq)
{
    if (_sources[irq])
    {
        return _sources[irq].vector();
    }

    return irq;
}

void processor::initialize()
{
    _bsp = processor::id();

    _lapic_base = memory::vm::allocate_address_range(4096);

    gdt::initialize();
    idt::initialize();

    initialize_exceptions();

    acpi::initialize();

//    remapping::initialize();

    acpi::parse_madt(_cores, _num_cores, _ioapics, _num_ioapics, _sources);
    lapic::initialize();

    gdt::ap_initialize();

    for (uint64_t i = 0; i < _num_ioapics; ++i)
    {
        _ioapics[i].initialize();

        if (_ioapics[i].end() > _max_ioapic_input)
        {
            _max_ioapic_input = _ioapics->end() - 1;
        }
    }

    time::hpet::initialize();
    if (!time::hpet::ready())
    {
        time::pit::initialize();
    }
    time::real::initialize();
    time::lapic::initialize();

    syscalls::initialize();

    smp::boot(_cores + 1, _num_cores - 1);
    memory::pmm::ap_initialize();
    smp::initialize_parallel();
    memory::drop_bootloader_mapping();

    _ready = true;
}

void processor::ap_initialize()
{
    while (!_ready)
    {
        asm volatile ("pause");
    }

    get_lapic()->ap_initialize();

    memory::pmm::ap_initialize();
    gdt::ap_initialize();
    idt::ap_initialize();
    wrmsr(0xc0000101, reinterpret_cast<uint64_t>(processor::current_core()));
    wrmsr(0xc0000102, reinterpret_cast<uint64_t>(processor::current_core()));
    time::lapic::ap_initialize();

    syscalls::ap_initialize();

    STI;

    for (;;)
    {
        HLT;
    }
}

processor::ioapic * processor::get_ioapic(uint8_t input)
{
    for (uint64_t i = 0; i < _num_ioapics; ++i)
    {
        if (input >= _ioapics[i].begin() && input < _ioapics[i].end())
        {
            return _ioapics + i;
        }
    }

    return nullptr;
}

processor::interrupt_entry * processor::sources()
{
    return _sources;
}

uint8_t processor::max_ioapic_input()
{
    return _max_ioapic_input;
}

processor::core * processor::get_core(uint64_t apic_id)
{
    for (uint64_t i = 0; i < _num_cores; ++i)
    {
        if (_cores[i].apic_id() == apic_id)
        {
            return _cores + i;
        }
    }

    PANIC("requested core with unknown APIC ID!");
}

uint64_t processor::id()
{
    if (get_lapic())
    {
        return get_lapic()->id();
    }

    return initial_id();
}

uint64_t processor::core_count()
{
    return _num_cores;
}

processor::core * processor::cores()
{
    return _cores;
}

processor::core * processor::current_core()
{
    if (unlikely(!scheduler::ready()))
    {
        return get_core(id());
    }

    return processor::current_thread()->current_core;
}

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

#include <processor/processor.h>
#include <processor/interrupts.h>
#include <memory/memory.h>
#include <acpi/acpi.h>
#include <processor/core.h>
#include <processor/ioapic.h>
#include <screen/screen.h>
#include <processor/current_core.h>
#include <processor/pit.h>
#include <processor/interrupt_entry.h>
#include <processor/smp.h>

namespace
{
    processor::core _cores[processor::max_cores];
    processor::ioapic _ioapics[processor::max_ioapics];
    
    uint64_t _num_cores = 0;
    uint64_t _num_ioapics = 0;
    
    processor::interrupt_entry _sources[128];
}

extern "C" void _load_gdt();
extern "C" void _load_idt();
extern "C" processor::gdt::gdt_entry _gdt_start[];

void processor::initialize()
{
    screen::transaction();

    gdt::initialize();
    idt::initialize();
    
    acpi::initialize();
    acpi::parse_madt(_cores, _num_cores, _ioapics, _num_ioapics, _sources);
    
    for (uint64_t i = 0; i < _num_ioapics; ++i)
    {
        _ioapics[i].initialize(_sources);
    }
    
    // TODO: HPET
    
/*    hpet::initialize();
    
    if (!hpet::present())
    {*/
        pit::initialize();
    /*}*/
    
    current_core::initialize();
    
    smp::boot(_cores + 1, _num_cores - 1);
    
//    memory::pmm::split_frame_stack(_cores, _num_cores);
}

void processor::ap_initialize()
{
    // TODO: stack management
    memory::stack::set(memory::stack::get());
    
    uint32_t apic_id = current_core::id();
    
    // TODO: map those little... erm... guys :D
    processor::gdt::gdt_entry * _core_gdt = (processor::gdt::gdt_entry *)(memory::vm::ap_gdt_area + apic_id * sizeof(*_core_gdt) * 7);
    processor::gdt::tss * _core_tss = (processor::gdt::tss *)(memory::vm::ap_tss_area + apic_id * sizeof(processor::gdt::tss));
    processor::idt::idtr * _core_idtr = (processor::idt::idtr *)(memory::vm::ap_idtr_area + apic_id * sizeof(processor::idt::idtr));
    processor::idt::idt_entry * _core_idt = (processor::idt::idt_entry *)(memory::vm::ap_idt_area + apic_id * 4096);
    
    current_core::initialize();
}

processor::ioapic & processor::get_ioapic(uint8_t irq)
{
    for (uint64_t i = 0; i < _num_ioapics; ++i)
    {
        if (irq >= _ioapics[i].begin() && irq < _ioapics[i].end())
        {
            return _ioapics[i];
        }
    }
    
    PANIC("Too high IRQ number requested");
    
    return *(ioapic *)nullptr;
}

uint8_t processor::translate_isa(uint8_t irq)
{
    if (_sources[irq])
    {
        return _sources[irq].vector();
    }
    
    return irq;
}

namespace
{
    processor::gdt::tss _tss;
    
    void _setup_gdte(uint64_t id, bool code, bool user, processor::gdt::gdt_entry * start = _gdt_start)
    {
        start[id].normal = 1;
        
        start[id].code = code;
        start[id].dpl = user * 3;
        start[id].long_mode = 1;
        start[id].present = 1;
        start[id].read_write = 1;
    }
    
    void _setup_tss(uint64_t id, processor::gdt::gdt_entry * start = _gdt_start, processor::gdt::tss * tss = &_tss)
    {
        memory::zero(&tss);
        
        tss->iomap = sizeof(processor::gdt::tss);
        
        start[id].base_low = (uint64_t)&tss & 0xFFFFFF;
        start[id].base_high = ((uint64_t)&tss >> 24) & 0xFF;
        *(uint32_t *)&start[id + 1] = ((uint64_t)&tss >> 32) & 0xFFFFFFFF;
        
        start[id].limit_low = (sizeof(processor::gdt::tss) & 0xFFFF) - 1;
        start[id].limit_high = sizeof(processor::gdt::tss) >> 16;
        
        start[id].accessed = 1;
        start[id].code = 1;
        start[id].present = 1;
        start[id].dpl = 3;
    }
}

void processor::gdt::initialize()
{
    memory::zero(_gdt_start, 7);
    
    _setup_gdte(1, true, false);
    _setup_gdte(2, false, false);
    _setup_gdte(3, true, true);
    _setup_gdte(4, false, true);
    _setup_tss(5);
    
    _load_gdt();
}

void processor::ipi(processor::core * core, processor::ipis ipi, uint8_t vector)
{
    processor::current_core::ipi(core->apic_id(), ipi, vector);
}

/*void processor::broadcast(processor::broadcast_types target, processor::current_core::ipis ipi, uint8_t vector)
{
    processor::current_core::broadcast(target, ipi, vector);
}*/

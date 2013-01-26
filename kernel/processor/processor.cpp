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

namespace
{
    processor::core cores[processor::max_cores];
    processor::ioapic ioapics[processor::max_ioapics];
    
    uint64_t num_cores = 0;
    uint64_t num_ioapics = 0;
}

extern "C" processor::gdt::gdt_entry gdt_start[];

void processor::initialize()
{
    screen::transaction();

    gdt::initialize();
    idt::initialize();
    
    acpi::initialize(cores, num_cores, ioapics, num_ioapics);
    
/*    apic::initialize(cores[0]);
    apic::initialize(ioapics, num_ioapics);
    
    apic::boot(cores + 1, num_cores - 1);
*/}

extern "C" void _load_gdt();

namespace
{
    processor::gdt::tss _tss;
    
    void setup_gdte(uint64_t id, bool code, bool user)
    {
        gdt_start[id].normal = 1;
        
        gdt_start[id].code = code;
        gdt_start[id].dpl = user * 3;
        gdt_start[id].long_mode = 1;
        gdt_start[id].present = 1;
    }
    
    void setup_tss(uint64_t id)
    {
        memory::zero(&_tss);
        
        _tss.iomap = sizeof(processor::gdt::tss);
        
        gdt_start[id].base_low = (uint64_t)&_tss & 0xFFFFFF;
        gdt_start[id].base_high = ((uint64_t)&_tss >> 24) & 0xFF;
        *(uint32_t *)&gdt_start[id + 1] = ((uint64_t)&_tss >> 32) & 0xFFFFFFFF;
        
        gdt_start[id].limit_low = (sizeof(processor::gdt::tss) & 0xFFFF) - 1;
        gdt_start[id].limit_high = sizeof(processor::gdt::tss) >> 16;
        
        gdt_start[id].accessed = 1;
        gdt_start[id].code = 1;
        gdt_start[id].present = 1;
        gdt_start[id].dpl = 3;
    }
}

void processor::gdt::initialize()
{
    memory::zero(gdt_start, 7);
    
    setup_gdte(1, true, false);
    setup_gdte(2, false, false);
    setup_gdte(3, true, true);
    setup_gdte(4, false, true);
    setup_tss(5);
    
    _load_gdt();
}

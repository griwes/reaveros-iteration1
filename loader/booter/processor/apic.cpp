/**
 * Reaver Project OS, Rose License
 * 
 * Copyright (C) 2011-2012 Reaver Project Team:
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

#include <processor/apic.h>

processor::apic_env::apic_env(acpi::madt * madt) : base(0), nmi_number(0), ioapics(nullptr), lapics(nullptr), 
    x2apics(nullptr)
{
    if (!madt)
    {        
        return;
    }
    
    acpi::madt_entry * entry = madt->entries;
    
    while ((uint64_t)entry - (uint64_t)madt < madt->length)
    {
        switch (entry->type)
        {
            case 0:
                // lapic
            case 1:
                // ioapic
            case 2:
                // int source override
            case 3:
                // NMI source
            case 4:
                // lapic nmi
            case 5:
                // address override
            case 9:
                // x2apic
            case 10:
                // x2apic nmi
        }
        
        entry = (acpi::madt_entry *)((uint64_t)entry + entry->length);
    }
}

void processor::apic_env::add_ioapic(processor::ioapic * io)
{
    if (!ioapics)
    {
        ioapics = io;
        
        return;
    }
    
    auto last = ioapics;
    
    while (last->next)
    {
        last = last->next;
    }
    
    last->next = io;
}

void processor::apic_env::add_lapic(processor::lapic * l)
{
    if (!lapics)
    {
        lapics = l;
        
        return;
    }
    
    auto last = lapics;
    
    while (last->next)
    {
        last = last->next;
    }
    
    last->next = l;
}

void processor::apic_env::add_x2apic(processor::x2apic * x2)
{
    if (!x2apics)
    {
        x2apics = x2;
        
        return;
    }
    
    auto last = x2apics;
    
    while (last->next)
    {
        last = last->next;
    }
    
    last->next = x2;
}

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
#include <screen/screen.h>

template<>
void screen::print_impl(const processor::apic_env & env)
{
    screen::line();
    screen::printfl("Local APIC memory address: 0x%016x", env.base);
    
    if (env.ioapics)
    {
        screen::printl("I/O APICs:");
        screen::printl(*env.ioapics);
    }
    
    if (env.lapics)
    {
        screen::printl("Local APICs:");
        screen::printl(*env.lapics);
    }
    
    if (env.x2apics)
    {
        screen::printl("x2 APICs:");
        screen::printl(*env.x2apics);
    }
}

template<>
void screen::print_impl(const processor::ioapic & ioapics)
{
    screen::printl("I/O APIC ID: ", ioapics.id);
    screen::printfl("I/O APIC base address: 0x%016x", ioapics.base_address);
    screen::printl("I/O APIC base interrupt number: ", ioapics.base_int);
    
    if (ioapics.next)
    {
        screen::printl(*ioapics.next);
    }
}

template<>
void screen::print_impl(const processor::lapic & lapics)
{
    screen::printl("Local APIC ID: ", lapics.apic_id);
    screen::printl("Local APIC ACPI ID: ", lapics.acpi_id);
    screen::printl("Local APIC NMI vector: ", lapics.nmi_int);
    
    if (lapics.next)
    {
        screen::printl(*lapics.next);
    }
}

template<>
void screen::print_impl(const processor::x2apic & x2apics)
{
    screen::printl("x2 Local APIC ID: ", x2apics.apic_id);
    screen::printl("x2 Local APIC ACPI UUID: ", x2apics.acpi_uuid);
    screen::printl("x2 Local APIC NMI vector: ", x2apics.nmi_int);
    
    if (x2apics.next)
    {
        screen::printl(*x2apics.next);
    }
}

processor::apic_env::apic_env(acpi::madt * madt) : base(0xfee00000), nmi_number(0), ioapics(nullptr), lapics(nullptr), 
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
            {
                auto lapic = (acpi::madt_lapic_entry *)((uint64_t)entry + sizeof(*entry));
                
                processor::lapic * l = new processor::lapic;
                l->acpi_id = lapic->acpi_id;
                l->apic_id = lapic->apic_id;
                
                add_lapic(l);
                
                break;
            }
            
            case 1:
            {
                auto ioapic = (acpi::madt_ioapic_entry *)((uint64_t)entry + sizeof(*entry));
                
                processor::ioapic * io = new processor::ioapic;
                
                io->base_address = ioapic->base_address;
                io->base_int = ioapic->base_int;
                io->id = ioapic->apic_id;
                
                add_ioapic(io);
                
                break;
            }
            
            case 2:
            {
                // auto override = (acpi::madt_int_override_entry *)((uint64_t)entry + sizeof(*entry));
                
                // TODO
                
                break;
            }
            
            case 3:
            {
                // auto nmi = (acpi::madt_nmi_source_entry *)((uint64_t)entry + sizeof(*entry));
                
                // TODO
                
                break;
            }
            
            case 4:
            {
                // auto lapic_nmi = (acpi::madt_lapic_nmi_entry *)((uint64_t)entry + sizeof(*entry));
                
                // TODO
                
                break;
            }
            
            case 5:
            {
                // auto override = (acpi::madt_lapic_address_override_entry *)((uint64_t)entry + sizeof(*entry));
                
                // TODO
                
                break;
            }
            
            case 9:
            {
                auto x2apic = (acpi::madt_x2apic_entry *)((uint64_t)entry + sizeof(*entry));
                
                processor::x2apic * x2 = new processor::x2apic;
                
                x2->acpi_uuid = x2apic->acpi_uuid;
                x2->apic_id = x2apic->x2apic_id;
                
                add_x2apic(x2);
                
                break;
            }

            case 10:
            {
                // auto x2apic_nmi = (acpi::madt_x2apic_nmi_entry *)((uint64_t)entry + sizeof(*entry));
                
                // TODO
                
                break;
            }
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

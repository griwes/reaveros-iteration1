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
#include <memory/memory.h>
#include <memory/x64paging.h>

template<>
void screen::print_impl(const processor::apic_env & env)
{
    screen::printfl("Local APIC memory address: 0x%016x", env.base);
    screen::line();
    
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
    screen::printl("I/O APIC number of interrupt vectors: ", ioapics.size);
    
    if (ioapics.next)
    {
        screen::line();
        screen::print(*ioapics.next);
    }
}

template<>
void screen::print_impl(const processor::lapic & lapics)
{
    screen::printl("Local APIC ID: ", lapics.apic_id);
    screen::printl("Local APIC ACPI ID: ", lapics.acpi_id);
    if (lapics.nmi_specified) screen::printl("Local APIC NMI vector: ", lapics.nmi_int);
    
    if (lapics.next)
    {
        screen::line();
        screen::print(*lapics.next);
    }
}

template<>
void screen::print_impl(const processor::x2apic & x2apics)
{
    screen::printl("x2 Local APIC ID: ", x2apics.apic_id);
    screen::printl("x2 Local APIC ACPI UUID: ", x2apics.acpi_uuid);
    if (x2apics.nmi_specified) screen::printl("x2 Local APIC NMI vector: ", x2apics.nmi_int);
    
    if (x2apics.next)
    {
        screen::line();
        screen::print(*x2apics.next);
    }
}

processor::apic_env::apic_env(acpi::madt * madt) : base(madt->lic_address)
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
                
                if (lapic->flags & 1)
                {
                    processor::lapic * l = new processor::lapic;
                    l->acpi_id = lapic->acpi_id;
                    l->apic_id = lapic->apic_id;
                
                    add_lapic(l);
                }
                
                break;
            }
            
            case 1:
            {
                auto ioapic = (acpi::madt_ioapic_entry *)((uint64_t)entry + sizeof(*entry));
                
                processor::ioapic * io = new processor::ioapic;
            
                io->base_int = ioapic->base_int;
                io->id = ioapic->apic_id;
                
                memory::vas->map(0xffff0000, 0xffff1000, ioapic->base_address);
                io->base_address = 0xffff0000;
                io->size = (io->read_register(1) >> 16) & 8;
                memory::vas->unmap(0xffff0000, 0xffff1000);
                    
                io->base_address = ioapic->base_address;
                
                add_ioapic(io);
                
                break;
            }
            
            case 3:
            {
                auto nmi = (acpi::madt_nmi_source_entry *)((uint64_t)entry + sizeof(*entry));
                
                add_global_nmi(nmi->int_number);
                
                break;
            }
            
            case 5:
            {
                //auto override = (acpi::madt_lapic_address_override_entry *)((uint64_t)entry + sizeof(*entry));
                
                //base = override->base_address;
                
                break;
            }
            
            case 9:
            {
                auto x2apic = (acpi::madt_x2apic_entry *)((uint64_t)entry + sizeof(*entry));
                
                if (x2apic->flags & 1)
                {
                    processor::x2apic * x2 = new processor::x2apic;
                
                    x2->acpi_uuid = x2apic->acpi_uuid;
                    x2->apic_id = x2apic->x2apic_id;
                
                    add_x2apic(x2);
                }
                
                break;
            }
        }
        
        entry = (acpi::madt_entry *)((uint64_t)entry + entry->length);
    }
    
    entry = madt->entries;
    
    while ((uint64_t)entry - (uint64_t)madt < madt->length)
    {
        switch (entry->type)
        {
            case 4:
            {
                auto lapic_nmi = (acpi::madt_lapic_nmi_entry *)((uint64_t)entry + sizeof(*entry));

                if (lapic_nmi->acpi_id == 0xff)
                {
                    auto entry = lapics;
                    
                    while (entry)
                    {
                        entry->nmi_int = lapic_nmi->int_number;
                        entry->nmi_specified = true;
                        entry->nmi_flags = lapic_nmi->flags;
                        
                        entry = entry->next;
                    }
                    
                    break;
                }
                
                auto lapic = get_lapic(lapic_nmi->acpi_id);
                
                if (!lapic)
                {
                    screen::print(" (ignoring LAPIC NMI entry for unknown LAPIC ACPI ID...) ");
                }
                
                else
                {
                    lapic->nmi_int = lapic_nmi->int_number;
                }
                
                break;
            }
            
            
            case 10:
            {
                auto x2apic_nmi = (acpi::madt_x2apic_nmi_entry *)((uint64_t)entry + sizeof(*entry));
                
                if (x2apic_nmi->acpi_uuid == 0xffffffff)
                {
                    for (auto entry = lapics; entry; entry = entry->next)
                    {
                        entry->nmi_int = x2apic_nmi->int_number;
                        entry->nmi_specified = true;
                        entry->nmi_flags = x2apic_nmi->flags;
                    }
                    
                    
                    for (auto entry = x2apics; entry; entry = entry->next)
                    {
                        entry->nmi_int = x2apic_nmi->int_number;
                        entry->nmi_specified = true;
                        entry->nmi_flags = x2apic_nmi->flags;
                    }
                    
                    break;
                }
                
                auto x2apic = get_x2apic(x2apic_nmi->acpi_uuid);
                
                if (!x2apic)
                {
                    screen::printl(" (ignoring x2APIC NMI entry for unknown x2APIC ACPI UUID...) ");
                }
                
                else
                {                    
                    x2apic->nmi_int = x2apic_nmi->int_number;
                }
                
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

processor::lapic * processor::apic_env::get_lapic(uint8_t id)
{
    auto entry = lapics;
    
    while (entry)
    {
        if (entry->acpi_id == id)
        {
            return entry;
        }
        
        entry = entry->next;
    }
    
    return nullptr;
}

processor::x2apic * processor::apic_env::get_x2apic(uint32_t id)
{
    auto entry = x2apics;
    
    while (entry)
    {
        if (entry->acpi_uuid == id)
        {
            return entry;
        }
        
        entry = entry->next;
    }
    
    return nullptr;
}

void processor::apic_env::add_global_nmi(uint32_t id)
{
    if (!global_nmis)
    {
        global_nmis = new nmi{id, nullptr};
        
        return;
    }
    
    auto entry = global_nmis;
    
    while (entry->next)
    {
        entry = entry->next;
    }
    
    entry->next = new nmi{id, nullptr};
}

namespace
{
    void _setup_io_apic(processor::ioapic * )//io)
    {
        
    }
}

void processor::setup_io_apics(processor::apic_env * apics)
{
    for (auto entry = apics->ioapics; entry; entry = entry->next)
    {
        _setup_io_apic(entry);
    }
}

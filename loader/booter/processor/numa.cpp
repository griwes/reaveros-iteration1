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

#include <processor/numa.h>
#include <acpi/tables.h>
#include <screen/screen.h>

template<>
void screen::print_impl(const processor::numa_domain &)
{
    // TODO
}

template<>
void screen::print_impl(const processor::numa_env & env)
{
    if (!env.size)
    {
        screen::printl("No NUMA domains present.");
    }
    
    screen::printl("Number of NUMA domains: ", env.size);
    
    auto domain = env.domains;
    for (uint32_t i = 0; i < env.size; ++i)
    {
        screen::printl(*domain);
        domain = domain->next;
    }
}

processor::numa_env::numa_env(acpi::srat * srat) : size(0), domains(nullptr)
{
    if (!srat)
    {        
        return;
    }
    
    acpi::srat_entry * entry = srat->entries;
    
    while ((uint64_t)entry - (uint64_t)srat < srat->length)
    {
        switch (entry->type)
        {
            case 0:
            {
                auto lapic = (acpi::srat_lapic_entry *)((uint64_t)entry + sizeof(*entry));
                
                numa_domain * domain = get_domain(lapic->domain | lapic->domain2 << 8 | lapic->domain3 << 16);
                domain->add_core(lapic->apic_id, lapic->flags);
                
                entry = (acpi::srat_entry *)((uint64_t)lapic + sizeof(*lapic)); 
                
                break;
            }
            case 1:
            {
                auto memory = (acpi::srat_memory_entry *)((uint64_t)entry + sizeof(*entry));
                
                numa_domain * domain = get_domain(memory->domain);
                domain->add_memory_range(memory->base, memory->length, memory->flags);
                
                entry = (acpi::srat_entry *)((uint64_t)memory + sizeof(*memory));
                
                break;
            }
            case 2:
            {
                auto x2apic = (acpi::srat_x2apic_entry *)((uint64_t)entry + sizeof(*entry));
                
                numa_domain * domain = get_domain(x2apic->domain);
                domain->add_core(x2apic->x2apic_id, x2apic->flags, true);
                
                entry = (acpi::srat_entry *)((uint64_t)x2apic + sizeof(*x2apic));
                
                break;
            }
            default:
                PANIC("unknown entry in SRAT");
        }
    }
}

processor::numa_domain * processor::numa_env::get_domain(uint32_t domain)
{
    auto current = domains;
    
    if (!current)
    {
        ++size;
        
        domains = new numa_domain;
        domains->id = domain;
        return domains;
    }
    
    for (uint32_t i = 0; i < size; ++i)
    {
        if (current->id == domain)
        {
            return current;
        }
        
        current = current->next;
    }
    
    ++size;
    
    current->next = new numa_domain;
    current->next->id = domain;
    return current->next;
}

void processor::numa_domain::add_core(uint32_t lapic_id, uint32_t flags, bool x2apic)
{
    if (flags != 0)
    {
        cores.add_core(lapic_id, x2apic);
    }
}

void processor::numa_domain::add_memory_range(uint64_t base, uint64_t length, uint32_t flags)
{
    if ((flags & 3) == 3)
    {
        memory.add_range(base, length);
    }
}

void processor::numa_cores::add_core(uint32_t lapic_id, bool x2apic)
{
    numa_core * c = new numa_core;
    c->lapic_id = lapic_id;
    c->x2apic_entry = x2apic;
    
    auto last = cores;
    
    for (uint32_t i = 0; i < size; ++i)
    {
        last = last->next;
    }
    
    last->next = c;
    ++size;
}

void processor::memory_ranges::add_range(uint64_t base, uint64_t end)
{
    memory_range * range = new memory_range;
    range->base = base;
    range->end = end;
    
    auto last = ranges;
    
    for (uint32_t i = 0; i < size; ++i)
    {
        last = last->next;
    }
    
    last->next = range;
    ++size;
}

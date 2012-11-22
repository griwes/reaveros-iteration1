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
#include <processor/apic.h>
#include <acpi/tables.h>
#include <screen/screen.h>

template<>
void screen::print_impl(const processor::numa_cores & cores)
{
    if (cores.cores)
    {
        auto core = cores.cores;

        while (core)
        {
            screen::printf("Core LAPIC ID: 0x%016x", core->lapic_id);
            screen::printl(", x2APIC: ", (core->x2apic_entry ? "yes" : "no"));
            core = core->next;
        }
    }
    
    else
    {
        screen::printl("<no cores>");
    }
}

template<>
void screen::print_impl(const processor::memory_ranges & ranges)
{
    if (ranges.ranges)
    {
        auto range = ranges.ranges;
        
        while (range)
        {
            screen::printf(" - 0x%016x", range->base); screen::printfl("- 0x%016x", range->end);
            range = range->next;
        }
    }
    
    else
    {
        screen::printl("<no memory>");
    }
}

template<>
void screen::print_impl(const processor::numa_domain & domain)
{
    screen::printfl("Domain ID: 0x%016x", domain.id);
    screen::printl("Cores:");
    screen::printl(domain.cores);
    screen::printl("Memory ranges:");
    screen::print(domain.memory);
}

template<>
void screen::print_impl(const processor::numa_env & env)
{
    if (!env.size)
    {
        screen::printl("No NUMA domains present.");
        
        return;
    }
    
    screen::printl("Number of NUMA domains: ", env.size);
    
    auto domain = env.domains;
    for (uint32_t i = 0; i < env.size; ++i)
    {
        screen::printl(*domain);
        domain = domain->next;
    }
}

namespace 
{
    processor::lapic * _find(processor::lapic * start, uint8_t needle)
    {
        while (start && start->apic_id != needle)
        {
            start = start->next;
        }
        
        return start;
    }
    
    processor::x2apic * _find(processor::x2apic * start, uint32_t needle)
    {
        while (start && start->apic_id != needle)
        {
            start = start->next;
        }
        
        return start;
    }
}

processor::numa_env::numa_env(acpi::srat * srat, processor::apic_env * apics) : size(0), domains(nullptr)
{
    get_domain(~0ull)->add_memory_range(0, ~0ull, 3);
    
    if (srat)
    {
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
                    
                    if (auto l = _find(apics->lapics, lapic->apic_id))
                    {
                        l->domain_specified = true;
                    }
                    
                    break;
                }
                
                case 1:
                {
                    auto memory = (acpi::srat_memory_entry *)((uint64_t)entry + sizeof(*entry));
                    
                    numa_domain * domain = get_domain(memory->domain);
                    domain->add_memory_range(memory->base, memory->length, memory->flags);
                    
                    break;
                }
                
                case 2:
                {
                    auto x2apic = (acpi::srat_x2apic_entry *)((uint64_t)entry + sizeof(*entry));
                    
                    numa_domain * domain = get_domain(x2apic->domain);
                    domain->add_core(x2apic->x2apic_id, x2apic->flags, true);
                    
                    if (auto x2 = _find(apics->x2apics, x2apic->x2apic_id))
                    {
                        x2->domain_specified = true;
                    }
                    
                    break;
                }
            }
            
            entry = (acpi::srat_entry *)((uint64_t)entry + entry->length); 
        }
    }
    
    for (auto lapic = apics->lapics; lapic; lapic = lapic->next)
    {
        if (!lapic->domain_specified)
        {                               
            get_domain(~0ull)->add_core(lapic->apic_id, 1);
        }        
    }
    
    for (auto x2apic = apics->x2apics; x2apic; x2apic = x2apic->next)
    {
        if (!x2apic->domain_specified)
        {
            get_domain(~0ull)->add_core(x2apic->apic_id, 1, true);
        }
    }
}

processor::numa_domain * processor::numa_env::get_domain(uint64_t domain)
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
    if (flags & 1)
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
    
    if (!cores)
    {
        cores = c;
        return;
    }
    
    auto last = cores;
    
    while (last->next)
    {
        last = last->next;
    }
    
    last->next = c;
}

void processor::memory_ranges::add_range(uint64_t base, uint64_t end)
{
    memory_range * range = new memory_range;
    range->base = base;
    range->end = end;

    if (!ranges)
    {
        ranges = range;
        return;
    }
    
    auto last = ranges;
    
    while (last->next)
    {
        last = last->next;
    }
    
    last->next = range;
}

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

processor::numa_env::numa_env(acpi::srat * srat) : size(0), domains(nullptr)
{
    acpi::srat_entry * entry = srat->entries;
    
    while ((uint64_t)entry - (uint64_t)srat < srat->length)
    {
        switch (entry->type)
        {
            case 0:
            {
                auto lapic = (acpi::srat_lapic_entry *)((uint64_t)entry + sizeof(*entry));
                
                //...
                
                entry = (acpi::srat_entry *)((uint64_t)lapic + sizeof(*lapic)); 
                
                break;
            }
            case 1:
            {
                auto memory = (acpi::srat_memory_entry *)((uint64_t)entry + sizeof(*entry));
                
                //...
                
                entry = (acpi::srat_entry *)((uint64_t)memory + sizeof(*memory));
                
                break;
            }
            case 2:
            {
                auto x2apic = (acpi::srat_x2apic_entry *)((uint64_t)entry + sizeof(*entry));
                
                //...
                
                entry = (acpi::srat_entry *)((uint64_t)x2apic + sizeof(*x2apic));
                
                break;
            }
            default:
                PANIC("unknown entry in SRAT");
        }
    }
}

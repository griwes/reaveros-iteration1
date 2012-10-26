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

#pragma once

namespace acpi
{
    struct srat;
}

namespace processor
{
    struct numa_core
    {
        numa_core() : lapic_id(0), x2apic_entry(false), next(nullptr) {}
        
        uint64_t lapic_id;
        bool x2apic_entry;
        
        numa_core * next;
    };
    
    struct numa_cores
    {
        numa_cores() : size(0), cores(nullptr) {}
        
        uint64_t size;
        numa_core * cores;
        
        void add_core(uint32_t, bool);
    };
    
    struct memory_range
    {
        memory_range() : base(0), end(0), next(nullptr) {}
        
        uint64_t base;
        uint64_t end;
        
        memory_range * next;
    };
    
    struct memory_ranges
    {
        memory_ranges() : size(0), ranges(nullptr) {};
        
        uint64_t size;
        memory_range * ranges;
        
        void add_range(uint64_t, uint64_t);
    };
    
    struct numa_domain
    {
        numa_domain() : id(0), next(nullptr) {}
        
        uint64_t id;
        memory_ranges memory;
        processor::numa_cores cores;
        
        numa_domain * next;
        
        void add_core(uint32_t, uint32_t, bool = false);
        void add_memory_range(uint64_t, uint64_t, uint32_t);
    };
    
    struct numa_env
    {
        numa_env(acpi::srat *);
        
        numa_domain * get_domain(uint32_t);
        
        uint32_t size;
        numa_domain * domains;
    };
}

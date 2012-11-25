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

#include <processor/cluster.h>
#include <screen/screen.h>

template<>
void screen::print_impl(const processor::cluster_env & clusters)
{
    screen::printl("Number of clusters: ", clusters.size);
    
    auto cluster = clusters.clusters;
    
    while (cluster)
    {
        screen::printl(*cluster);
        cluster = cluster->next;
    }
}

template<>
void screen::print_impl(const processor::cluster & cluster)
{
    screen::printl("Logical processors:");
    
    auto cpu = cluster.cores;
    
    while (cpu)
    {
        screen::printf("Core LAPIC ID: 0x%016x", cpu->apic_id);
        screen::printl(", x2APIC: ", (cpu->x2apic ? "yes" : "no"));
        cpu = cpu->next;
    }
    
    screen::printl("Cluster memory map:");
    screen::print(cluster.memory);
}

namespace 
{
    bool _check(processor::numa_env * numa)
    {
        for (auto domain = numa->domains; domain; domain = domain->next)
        {
            if (processor::_detail::_count(domain->cores.cores) > 16)
            {
                return false;
            }
        }
        
        return true;
    }
    
    void _add_core(processor::core *& base, processor::core * n)
    {
        if (!base)
        {
            base = n;
            return;
        }
        
        processor::core * b = base;
        
        while (b->next)
        {
            b = b->next;
        }
        
        b->next = n;
    }
}

processor::cluster_env::cluster_env(processor::numa_env * numa, memory::map * memmap)
{
    if (_check(numa))
    {
        // split memory map, TODO
        
        for (auto domain = numa->domains; domain; domain = domain->next)
        {
            processor::core * cores = nullptr;
            
            for (auto c = domain->cores.cores; c; c = c->next)
            {
                _add_core(cores, new core{c->lapic_id, c->x2apic_entry, nullptr});
            }
            
            add_cluster(cores, *memmap);
        }
        
        return;
    }
    
    // splitting logic, TODO
}

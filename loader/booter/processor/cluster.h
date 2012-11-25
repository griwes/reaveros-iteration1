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

#include <memory/memmap.h>
#include <memory/x64paging.h>

namespace processor
{
    namespace _detail
    {
        template<typename T>
        uint32_t _count(T * t)
        {
            uint32_t i = 0;
            for (; t; ++i, t = t->next);
            return i;
        }
    }
    
    struct core
    {
        uint64_t apic_id;
        bool x2apic;
        
        core * next;
    };
    
    struct cluster
    {
        core * cores;
        ::memory::map memory;
        uint32_t size;
        
        cluster * next;
        
        ::memory::x64::pml4 * vas;
    };
    
    struct cluster_env
    {
        cluster_env(processor::numa_env *, memory::map *);
        
        void add_cluster(core * cores, const memory::map & map)
        {
            ++size;

            if (!clusters)
            {
                clusters = new cluster{cores, map, _detail::_count(cores), nullptr, nullptr};
                return;
            }
            
            auto c = clusters;
            
            while (c)
            {
                c = c->next;
            }
            
            c->next = new cluster{cores, map, _detail::_count(cores), nullptr, nullptr};
        }
        
        cluster * clusters;
        uint32_t size;
    };
}
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

#include <memory/memory.h>
#include <memory/manager.h>
#include <processor/processor.h>
#include <memory/x64paging.h>
#include <processor/cluster.h>

namespace memory
{
    manager::allocator * default_allocator;
    x64::pml4 * vas;
}

void memory::initialize(uint32_t placement, map & memory_map)
{
    default_allocator = manager::make_placement_allocator(placement, memory_map);
}

void memory::prepare_long_mode()
{
    default_allocator->align(4096);
    vas = new x64::pml4;
    
    // identity map first 64 MiB
    x64::pdpt * table = new x64::pdpt;
    (*vas)[0] = table;
    
    x64::page_directory * pd = new x64::page_directory;
    (*table)[0] = pd;
    
    for (uint32_t i = 0; i < 32; ++i)
    {
        x64::page_table * pt = new x64::page_table;
        (*pd)[i] = pt;
        
        for (uint32_t j = 0; j < 512; ++j)
        {
            (*pt)[j] = i * 2 * 1024 * 1024 + j * 4 * 1024;
        }
    }
    
    processor::set_cr3((uint32_t)vas);
}

void memory::prepare_address_spaces(processor::cluster_env * clusters)
{
    auto def_alloc = default_allocator;
    
    for (auto cluster = clusters->clusters; cluster; cluster = cluster->next)
    {
        default_allocator = manager::make_placement_allocator((uint32_t)new int, cluster->memory);
    }
    
    default_allocator = def_alloc;
}

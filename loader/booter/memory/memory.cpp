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

void * operator new(size_t, void *);

namespace memory
{
    manager::placement_allocator default_allocator(0);
    x64::pml4 vas;
}

void memory::initialize(uint32_t placement, map_entry * memory_map, uint32_t memory_map_length)
{
    for (uint32_t i = 0; i < memory_map_length; ++i)
    {
        if (memory_map[i].type > 1)
        {
            memory_map[i].type += 4;
        }
        
        memory::map::add_entry(memory_map[i]);
    }
    
    new ((void *)&default_allocator) manager::placement_allocator(placement);
}

void memory::prepare_long_mode()
{
    default_allocator.align(4096);
    
    // identity map first 64 MiB
    x64::pdpt * table = new x64::pdpt;
    vas[0] = table;
    
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
    
    processor::set_cr3((uint32_t)&vas);
}

uint64_t memory::install_kernel(uint32_t kernel_base, uint32_t kernel_length)
{
    kernel_length *= 512;
    
    memory::default_allocator.align(4096);
    uint8_t * kernel = new uint8_t[kernel_length];

    copy((uint8_t *)kernel_base, kernel, kernel_length);

    vas.map(0xFFFFFFFF80000000, 0xFFFFFFFF80000000 + kernel_length, (uint64_t)kernel);
    
    return 0xFFFFFFFF80000000 + ((kernel_length + 4095) & ~(uint64_t)4095);
}

void memory::install_initrd(uint32_t kernel_end, uint32_t initrd_base, uint32_t initrd_length)
{
    initrd_length *= 512;
    
    memory::default_allocator.align(4096);
    uint8_t * initrd = new uint8_t[initrd_length];
    
    copy((uint8_t *)initrd_base, initrd, initrd_length);
    
    vas.map(kernel_end, kernel_end + initrd_length, (uint64_t)initrd);
}

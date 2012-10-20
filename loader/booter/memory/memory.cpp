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

#include "memory.h"
#include "manager.h"
#include "x86paging.h"
#include "../processor/processor.h"

namespace memory
{
    manager::allocator * default_allocator;
}

void memory::initialize(uint32_t placement, map & memory_map)
{
    default_allocator = manager::make_placement_allocator(placement, memory_map);
}

void memory::init_protected_paging()
{
    init_pat();
    
    default_allocator->align(4096);
    
    auto pd = new x86::page_directory;
    
    for (uint32_t i = 0; i < 1023; ++i)
    {
        auto pt = new x86::page_table(i);
        pd->entries[i] = pt;
    }
        
    pd->entries[1023] = (x86::page_table *)pd;
        
    processor::set_cr3(pd);
    processor::enable_paging();

    uint32_t vidmemstart = screen::output->video_start();
    uint32_t vidmemend = screen::output->video_end();
    
    while (vidmemstart < vidmemend)
    {
        x86::set_cache(vidmemstart, x86::write_combining);
        vidmemstart += 4096;
    }
}

void memory::init_pat()
{
    uint32_t low, high;
    rdmsr(0x277, low, high);
    
    low = 0;
    high = 0;
    
    // PAT0: Uncacheable = 0
    low |= 0x0;
    // PAT1: Write Combining = 1
    low |= 0x1 << 8;
    // PAT2: Write Through = 4
    low |= 0x4 << 16;
    // PAT3: Write Protected = 5
    low |= 0x5 << 24;
    // PAT4: Write Back = 6
    high |= 0x6;
    // PAT5: Uncached = 7
    high |= 0x7 << 8;
    // PAT6, PAT7: Uncacheable = 0
    
    wrmsr(0x277, low, high);
}

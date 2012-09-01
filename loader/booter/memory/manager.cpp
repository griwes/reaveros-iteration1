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

#include "manager.h"

void * operator new(uint32_t, void *);

memory::manager::allocator * memory::manager::make_placement_allocator(uint32_t placement, map & memory_map)
{
    placement += 15;
    placement &= ~(uint32_t)15;
    
    return new((void *)placement) placement_allocator((placement + sizeof(placement_allocator) + 15) & ~(uint32_t)15,
            memory_map);
}

memory::manager::placement_allocator::placement_allocator(uint32_t placement, memory::map & memory_map)
    : memory_map(memory_map), placement_address(placement)
{
}

memory::manager::placement_allocator::~placement_allocator()
{
}

void * memory::manager::placement_allocator::allocate(uint32_t size)
{
    size += 15;
    size &= ~(uint32_t)15;
    
    while (!memory_map.usable(placement_address) || !memory_map.usable(placement_address + size -1 ))
    {
        placement_address = memory_map.next_usable(placement_address);
        
        if (placement_address == 0)
        {
            PANIC("Not enough memory installed on the system.");
        }
    }
    
    auto ret = placement_address;
    
    return (void *)ret;
}

void memory::manager::placement_allocator::deallocate(void *)
{
    // no-op for placement
    
    return;
}

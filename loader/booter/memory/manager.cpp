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

#include <memory/manager.h>
#include <memory/memory.h>
#include "x64paging.h"

void * operator new(uint32_t, void *);

memory::manager::allocator * memory::manager::make_placement_allocator(uint32_t placement)
{
    placement += 15;
    placement &= ~(uint32_t)15;
    
    return new((void *)placement) placement_allocator((placement + sizeof(placement_allocator) + 15) & ~(uint32_t)15);
}

memory::manager::placement_allocator::placement_allocator(uint32_t placement)
    : placement_address(placement), /*base(placement), type(5),*/ top_mapped(64 * 1024 * 1024 - 1)
{
}

memory::manager::placement_allocator::~placement_allocator()
{
}

void * memory::manager::placement_allocator::allocate(uint32_t size)
{
    static bool vas_context = false;

    if (top_mapped - placement_address <= 3 * 4096 && !vas_context)
    {
        vas_context = true;
        
        vas->map(top_mapped + 1, top_mapped + 1 + 64 * 1024 * 1024, top_mapped + 1);
        top_mapped += 64 * 1024 * 1024;
        
        vas_context = false;
    }
    
    size += 15;
    size &= ~(uint32_t)15;
    
    while (!memory::map::usable(placement_address) || !memory::map::usable(placement_address + size -1 ))
    {
        placement_address = memory::map::next_usable(placement_address);
        
        if (placement_address == 0)
        {
            PANIC("Not enough memory installed on the system.");
        }
    }
    
    auto ret = placement_address;
    placement_address += size;
    
    return (void *)ret;
}

void memory::manager::placement_allocator::deallocate(void *)
{
    // no-op for placement
    
    return;
}

memory::manager::backwards_allocator::backwards_allocator()
{
}

memory::manager::backwards_allocator::~backwards_allocator()
{
}

void * memory::manager::backwards_allocator::allocate(uint32_t size)
{
    uint32_t top_placement = memory::map::find_last_usable(size);
    return (void *)top_placement;
}

void memory::manager::backwards_allocator::deallocate(void *)
{
    // no-op for backwards
    
    return;
}

void memory::manager::placement_allocator::align(uint32_t a)
{
    placement_address += a - 1;
    placement_address &= ~(a - 1);
}

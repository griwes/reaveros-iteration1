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

#include "memmap.h"

memory::map_t::map_t(memory::map_entry_t * base_map, uint32_t map_size)
    : sequence_entries(base_map), entries(nullptr), num_entries(map_size)
{
}

memory::map_t::~map_t()
{

}

bool memory::map_t::usable(uint64_t addr, uint32_t domain)
{
    if (entries)
    {
        auto entry = entries;
        
        for (uint64_t i = 0; i < num_entries; i++)
        {
            if (addr >= entry->base && addr < entry->base + entry->length)
            {
                if (entry->type == 1)
                {
                    if (domain != ~(uint32_t)0)
                    {
                        if (entry->proximity_domain == domain)
                        {
                            return true;
                        }
                    
                        return false;
                    }
                
                    return true;
                }
                
                return false;
            }
        }
        
        return false;
    }
    
    else
    {
        auto entry = sequence_entries;
        
        for (uint64_t i = 0; i < num_entries; i++)
        {
            if (addr >= entry->base && addr < entry->base + entry->length)
            {
                if (entry->type == 1)
                {
                    return true;
                }
                
                return false;
            }
        }
        
        return false;
    }
}

uint64_t memory::map_t::next_usable(uint64_t addr, uint32_t domain)
{
    if (entries)
    {
        auto entry = entries;
        
        for (uint64_t i = 0; i < num_entries; i++)
        {
            if (entry->base > addr)
            {
                if (entry->type == 1)
                {
                    if (domain != ~(uint32_t)0)
                    {
                        if (entry->proximity_domain == domain)
                        {
                            return entry->base;
                        }
                        
                        continue;
                    }
                    
                    return entry->base;
                }
            }
        }
        
        return 0;
    }
    
    else
    {
        auto entry = sequence_entries;
        
        for (uint64_t i = 0 ; i < num_entries; i++)
        {
            if (entry->base > addr)
            {
                if (entry->type == 1)
                {
                    return entry->base;
                }
            }
        }
        
        return 0;
    }
}

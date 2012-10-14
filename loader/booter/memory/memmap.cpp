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
#include "memory.h"

memory::map::map() : _sequence_entries(nullptr), _entries(nullptr), _num_entries(0)
{
}

memory::map::map(memory::map_entry * base_map, uint32_t map_size)
    : _sequence_entries(base_map), _entries(nullptr), _num_entries(map_size)
{
}

memory::map::~map()
{

}

bool memory::map::usable(uint64_t addr, uint32_t domain)
{
    if (_entries)
    {
        auto entry = _entries;
        
        for (uint32_t i = 0; i < _num_entries; ++i)
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
            
            entry = entry->next;
        }
        
        return false;
    }
    
    else
    {
        auto entry = _sequence_entries;
        
        for (uint32_t i = 0; i < _num_entries; ++i)
        {
            if (addr >= entry->base && addr < entry->base + entry->length)
            {
                if (entry->type == 1)
                {
                    return true;
                }
                
                return false;
            }
            
            ++entry;
        }
        
        return false;
    }
}

uint64_t memory::map::next_usable(uint64_t addr, uint32_t domain)
{
    if (_entries)
    {
        auto entry = _entries;
        
        for (uint32_t i = 0; i < _num_entries; ++i)
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
            
            entry = entry->next;
        }
        
        return 0;
    }
    
    else
    {
        auto entry = _sequence_entries;
        
        uint64_t lowest = 0;
        
        for (uint32_t i = 0; i < _num_entries; ++i)
        {
            if (entry->base > addr)
            {
                if (entry->type == 1)
                {
                    if (!lowest || lowest > entry->base)
                    {
                        lowest = entry->base;
                    }
                }
            }
            
            ++entry;
        }
        
        return lowest;
    }
}

void print(memory::map_entry * entry)
{
    screen::printf("| 0x%016x | 0x%016x | ", entry->base, entry->length);
    
    switch (entry->type)
    {
        case 1:
            screen::print("Free memory                ");
            break;
        case 2:
            screen::print("Reserved memory            ");
            break;
        case 3:
            screen::print("ACPI reclaimable memory    ");
            break;
        case 4:
            screen::print("ACPI NVS memory            ");
            break;
        case 5:
            screen::print("Bad memory                 ");
            break;
        case 6:
            screen::print("ISA DMA memory             ");
            break;
        case 7:
            screen::print("Kernel memory              ");
            break;
        case 8:
            screen::print("Initrd memory              ");
            break;
        case 9:
            screen::print("Video backbuffer           ");
            break;
        case 10:
            screen::print("Booter memory              ");
            break;
    }
    
    screen::printl(" |");
}

template<>
void screen::print(const memory::map & map)
{
    printl("|--------------------|--------------------|-----------------------------|");
    printl("| Base address       | Length             | Type                        |");
    printl("|--------------------|--------------------|-----------------------------|");
    
    if (map._entries)
    {
        auto entry = map._entries;
        
        for (uint32_t i = 0; i < map._num_entries; ++i)
        {
            ::print(entry);
            entry = entry->next;
        }
    }
    
    else
    {
        auto entry = map._sequence_entries;
        
        for (uint32_t i = 0; i < map._num_entries; ++i)
        {
            ::print(entry);
            ++entry;
        }
    }

    printl("|--------------------|--------------------|-----------------------------|");
}

memory::map * memory::map::sanitize() // and sort, don't forget sorting!
{
    // linked-list map is not sanitizeable, because only initial memory map can be insane (let's hope so)
    if (_entries)
    {
        return nullptr;
    }
    
    map * sane_map = new memory::map;

    for (uint32_t i = 0; i < _num_entries; ++i)
    {
        sane_map->add_entry(new chained_map_entry(&_sequence_entries[i]));
    }
    
    chained_map_entry * isa_dma = new chained_map_entry();
    isa_dma->base = 1024 * 1024;
    isa_dma->length = 15 * 1024 * 1024;
    isa_dma->type = 6;
    
    sane_map->add_entry(isa_dma);
    
    return sane_map;
}

namespace 
{
    inline bool _intersect(memory::chained_map_entry * first, memory::chained_map_entry * second)
    {
        return !(first->base + first->length <= second->base || second->base + second->length <= first->base);
    }
    
    bool _this_or_next(memory::chained_map_entry * first, memory::chained_map_entry * second)
    {
        if (!second->next || _intersect(first, second))
        {
            return true;
        }
        
        else if (second->base < first->base)
        {
            return false;
        }
        
        else
        {
            return true;
        }
    }
    
    memory::chained_map_entry * _combine_entries(memory::chained_map_entry *, memory::chained_map_entry *)
    {
        return nullptr;
    }
}

void memory::map::add_entry(memory::chained_map_entry * entry)
{
    if (_sequence_entries)
    {
        PANIC("Trying to add chained entry to sequenced memory map!");
    }
        
    if (_num_entries == 0)
    {
        _entries = entry;
        
        _num_entries++;
        
        return;
    }
    
    else
    {
        auto _entry = _entries;
        
        if (entry->base <= _entry->base)
        {
            if (auto e = _combine_entries(entry, _entry))
            {
                _entries = e;
                
                _num_entries++;
            }
            
            return;
        }
        
        for (uint64_t i = 0; i < _num_entries; ++i)
        {
            if (_this_or_next(_entry, entry))
            {
                if (_combine_entries(_entry, entry))
                {
                    _num_entries++;
                    
                    return;
                }
            }
        }
    }
}

uint32_t memory::map::find_last_usable(uint32_t size)
{
    if (_entries)
    {
        PANIC("find_last_usable is not supposed to be called after memory map sanitizing.");
    }
    
    for (uint32_t i = size; i > 0; --i)
    {
        auto entry = &_entries[i - 1];
        
        if (entry->base > 0xFFFFFFFF || entry->type != 1)
        {
            continue;
        }
        
        if (entry->length == size)
        {
            return entry->base;
        }
            
        if (entry->length > size)
        {
            return entry->base + (entry->length - size);
        }
    }
    
//    PANIC("call to find_last_usable failed.");
    
    return 0;
}

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
#include <memory/memory.h>

memory::map::map() : _sequence_entries(nullptr), _entries(nullptr), _num_entries(0)
{
}

memory::map::map(memory::map_entry * base_map, uint32_t map_size)
    : _sequence_entries(base_map), _entries(nullptr), _num_entries(map_size)
{
    for (uint32_t i = 0; i < _num_entries; ++i)
    {
        if (_sequence_entries[i].type != 1)
        {
            _sequence_entries[i].type += 4;
        }
    }
}

memory::map::~map()
{

}

bool memory::map::usable(uint64_t addr)
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

uint64_t memory::map::next_usable(uint64_t addr)
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
            screen::print("Kernel memory              ");
            break;
        case 3:
            screen::print("Initrd memory              ");
            break;
        case 4:
            screen::print("Video backbuffer           ");
            break;
        case 5:
            screen::print("Booter memory              ");
            break;
        case 6:
            screen::print("Reserved memory            ");
            break;
        case 7:
            screen::print("ACPI reclaimable memory    ");
            break;
        case 8:
            screen::print("ACPI NVS memory            ");
            break;
        case 9:
            screen::print("Bad memory                 ");
            break;
    }
    
    screen::printl(" |");
}

template<>
void screen::print_impl(const memory::map & map)
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
    
    return sane_map;
}

void memory::map::_combine_entries(memory::chained_map_entry * entry, memory::chained_map_entry * sequence)
{
    // 1) entry is below first in sequence, swap them and insert
    if (entry->base + entry->length <= sequence->base)
    {        
        entry->base ^= sequence->base ^= entry->base ^= sequence->base;
        entry->length ^= sequence->length ^= entry->length ^= sequence->length;
        entry->type ^= sequence->type ^= entry->type ^= sequence->type;
        
        if (sequence->next)
        {
            sequence->next->prev = entry;
        }
        
        entry->next = sequence->next;
        sequence->next = entry;
        entry->prev = sequence;
        
        ++_num_entries;
        
        return;
    }
    
    while (sequence->next)
    {
        // 2) entry is between two entries in sequence, insert
        if (sequence->base + sequence->length <= entry->base && entry->base + entry->length <= sequence->next->base)
        {
            entry->next = sequence->next;
            entry->prev = sequence;
            sequence->next->prev = entry;
            sequence->next = entry;
            
            ++_num_entries;
            
            return;
        }

        // 3) element in sequence kills entry - swap values and proceed with 4)
        if (entry->base >= sequence->base && entry->base + entry->length <= sequence->base + sequence->length)
        {
            entry->base ^= sequence->base ^= entry->base ^= sequence->base;
            entry->length ^= sequence->length ^= entry->length ^= sequence->length;
            entry->type ^= sequence->type ^= entry->type ^= sequence->type;
        }
        
        // 4) entry "kills" element of sequence 
        if (entry->base <= sequence->base && entry->base + entry->length >= sequence->base + sequence->length)
        {
            if (entry->type >= sequence->type)
            {
                if (sequence->prev)
                {
                    sequence->prev->next = sequence->next;
                }
                
                else
                {
                    _entries = sequence->next;
                }
                
                if (sequence->next)
                {
                    sequence->next->prev = sequence->prev;
                }
                
                --_num_entries;
                _combine_entries(entry, _entries);
                
                return;
            }
            
            else
            {
                chained_map_entry * second = new chained_map_entry;
                second->type = entry->type;
                second->base = sequence->base + sequence->length;
                second->length = entry->base + entry->length - second->base;
                entry->length -= second->length - sequence->length;
                
                _combine_entries(entry, _entries);
                _combine_entries(second, _entries);
                
                return;
            }
        }
        
        // 5) only parts of entries overlap
        if (entry->base + entry->length > sequence->base && entry->base < sequence->base)
        {
            entry->base ^= sequence->base ^= entry->base ^= sequence->base;
            entry->length ^= sequence->length ^= entry->length ^= sequence->length;
            entry->type ^= sequence->type ^= entry->type ^= sequence->type;
        }
        
        if (sequence->base + sequence->length > entry->base && entry->base > sequence->base)
        {
            if (sequence->type >= entry->type)
            {
                entry->length -= sequence->base + sequence->length - entry->base;
                entry->base = sequence->base + sequence->length;
                
                _combine_entries(entry, _entries);
                
                return;
            }
            
            else
            {
                sequence->length -= sequence->base + sequence->length - entry->base;
                
                _combine_entries(entry, _entries);
                
                return;
            }
        }
        
        sequence = sequence->next;
    }
    
    // 5) entry is above last in sequence
    sequence->next = entry;
    entry->prev = sequence;
    
    ++_num_entries;
}

void memory::map::_merge_siblings(memory::chained_map_entry * sequence)
{
    if (!sequence->next)
    {
        return;
    }
    
    auto first = sequence;
    auto second = first->next;
    
    while (second->next)
    {
        if (first->type == second->type && first->base + first->length == second->base)
        {
            first->length += second->length;
            first->next = second->next;
            second->next->prev = first;
            
            second = first->next;
            
            --_num_entries;
        }
        
        else
        {
            first = second;
            second = second->next;
        }
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
        _combine_entries(entry, _entries);
    }

    _merge_siblings(_entries);
}

uint32_t memory::map::find_last_usable(uint32_t size)
{
    if (_entries)
    {
        PANIC("find_last_usable is not supposed to be called after memory map sanitizing.");
    }
    
    size += 4096 - size % 4096;
    
    auto entry = &_sequence_entries[_num_entries - 1];
    
    while (entry >= _sequence_entries)
    {
        if (entry->base > 0xFFFFFFFFu - size || entry->type != 1)
        {
            --entry;
            continue;
        }
        
        if (entry->length == size)
        {
            return (uint32_t)entry->base;
        }
            
        if (entry->length > size)
        {
            entry->length -= size;
            return (uint32_t)(entry->base + entry->length);
        }
        
        --entry;
    }
    
    PANIC("call to find_last_usable failed.");
    
    return 0;
}

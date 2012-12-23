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

memory::map_entry memory::map::_entries[512];

bool memory::map::usable(uint64_t addr)
{
    for (uint32_t i = 0; i < _num_entries; ++i)
    {
        if (addr >= _entries[i].base && addr < _entries[i].base + _entries[i].length)
        {
            if (_entries[i].type == 1)
            {
                return true;
            }
            
            return false;
        }
    }
    
    return false;
}

uint64_t memory::map::next_usable(uint64_t addr)
{
    uint64_t lowest = 0;
    
    for (uint32_t i = 0; i < _num_entries; ++i)
    {
        if (_entries[i].base > addr)
        {
            if (_entries[i].type == 1)
            {
                if (!lowest || lowest > _entries[i].base)
                {
                    lowest = _entries[i].base;
                }
            }
        }
    }
    
    return lowest;
}

void print(memory::map_entry * entry)
{
    screen::printf("| 0x%016x ", entry->base);
    screen::printf("| 0x%016x | ", entry->length);
    
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

void memory::print_map()
{
    screen::printl("|--------------------|--------------------|-----------------------------|");
    screen::printl("| Base address       | Length             | Type                        |");
    screen::printl("|--------------------|--------------------|-----------------------------|");
    
    if (memory::map::_entries)
    {
        for (uint32_t i = 0; i < memory::map::_num_entries; ++i)
        {
            ::print(memory::map::_entries + i);
        }
    }
    
    screen::printl("|--------------------|--------------------|-----------------------------|");
}

void memory::map::_combine_entries(uint64_t i, uint64_t j)
{
    // 1) entry is below first in sequence, swap them and insert
    if (_entries[i].base + _entries[i].length <= _entries[j].base)
    {        
        _entries[i].base ^= _entries[j].base ^= _entries[i].base ^= _entries[j].base;
        _entries[i].length ^= _entries[j].length ^= _entries[i].length ^= _entries[j].length;
        _entries[i].type ^= _entries[j].type ^= _entries[i].type ^= _entries[j].type;
        
        if (_entries[j].next)
        {
            _entries[j].next->prev = entry;
        }
        
        _entries[i].next = _entries[j].next;
        _entries[j].next = entry;
        _entries[i].prev = sequence;
        
        ++_num_entries;
        
        return;
    }
    
    while (_entries[j].next)
    {
        // 2) entry is between two entries in sequence, insert
        if (_entries[j].base + _entries[j].length <= _entries[i].base && _entries[i].base + _entries[i].length <= _entries[j].next->base)
        {
            _entries[i].next = _entries[j].next;
            _entries[i].prev = sequence;
            _entries[j].next->prev = entry;
            _entries[j].next = entry;
            
            ++_num_entries;
            
            return;
        }

        // 3) element in sequence kills entry - swap values and proceed with 4)
        if (_entries[i].base >= _entries[j].base && _entries[i].base + _entries[i].length <= _entries[j].base + _entries[j].length)
        {
            _entries[i].base ^= _entries[j].base ^= _entries[i].base ^= _entries[j].base;
            _entries[i].length ^= _entries[j].length ^= _entries[i].length ^= _entries[j].length;
            _entries[i].type ^= _entries[j].type ^= _entries[i].type ^= _entries[j].type;
        }
        
        // 4) entry "kills" element of sequence 
        if (_entries[i].base <= _entries[j].base && _entries[i].base + _entries[i].length >= _entries[j].base + _entries[j].length)
        {
            if (_entries[i].type >= _entries[j].type)
            {
                if (_entries[j].prev)
                {
                    _entries[j].prev->next = _entries[j].next;
                }
                
                else
                {
                    _entries = _entries[j].next;
                }
                
                if (_entries[j].next)
                {
                    _entries[j].next->prev = _entries[j].prev;
                }
                
                --_num_entries;
                _combine_entries(entry, _entries);
                
                return;
            }
            
            else
            {
                chained_map_entry * second = new chained_map_entry;
                second->type = _entries[i].type;
                second->base = _entries[j].base + _entries[j].length;
                second->length = _entries[i].base + _entries[i].length - second->base;
                _entries[i].length -= second->length - _entries[j].length;
                
                _combine_entries(entry, _entries);
                _combine_entries(second, _entries);
                
                return;
            }
        }
        
        // 5) only parts of entries overlap
        if (_entries[i].base + _entries[i].length > _entries[j].base && _entries[i].base < _entries[j].base)
        {
            _entries[i].base ^= _entries[j].base ^= _entries[i].base ^= _entries[j].base;
            _entries[i].length ^= _entries[j].length ^= _entries[i].length ^= _entries[j].length;
            _entries[i].type ^= _entries[j].type ^= _entries[i].type ^= _entries[j].type;
        }
        
        if (_entries[j].base + _entries[j].length > _entries[i].base && _entries[i].base > _entries[j].base)
        {
            if (_entries[j].type >= _entries[i].type)
            {
                _entries[i].length -= _entries[j].base + _entries[j].length - _entries[i].base;
                _entries[i].base = _entries[j].base + _entries[j].length;
                
                _combine_entries(entry, _entries);
                
                return;
            }
            
            else
            {
                _entries[j].length -= _entries[j].base + _entries[j].length - _entries[i].base;
                
                _combine_entries(entry, _entries);
                
                return;
            }
        }
        
        sequence = _entries[j].next;
    }
    
    // 5) entry is above last in sequence
    _entries[j].next = entry;
    _entries[i].prev = sequence;
    
    ++_num_entries;
}

void memory::map::_merge_siblings(memory::chained_map_entry * sequence)
{
    if (!_entries[j].next)
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

void memory::map::add_entry(memory::map_entry * entry)
{
    if (_num_entries == 0)
    {
        _entries[0] = entry;
        
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
    
    for (uint32_t i = _num_entries; i >= 0; --i)
    {
        if (_entries[i].base > 0xFFFFFFFFu - size || _entries[i].type != 1)
        {
            continue;
        }
        
        if (_entries[i].length == size)
        {
            return (uint32_t)_entries[i].base;
        }
            
        if (_entries[i].length > size)
        {
            _entries[i].length -= size;
            return (uint32_t)(_entries[i].base + _entries[i].length);
        }
    }
    
    PANIC("call to find_last_usable failed.");
    
    return 0;
}

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

memory::map_entry memory::map::_entries[512] = {};
uint32_t memory::map::_num_entries = 0;

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

void print(memory::map_entry & entry)
{
    screen::printf("| 0x%016x ", entry.base);
    screen::printf("| 0x%016x | ", entry.length);
    
    switch (entry.type)
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
            ::print(memory::map::_entries[i]);
        }
    }
    
    screen::printl("|--------------------|--------------------|-----------------------------|");
}

void memory::map::_expand(uint32_t i)
{
    if (i >= _num_entries)
    {
        PANIC("Insane argument for _expand.");
    }
    
    if (_num_entries + 1 >= 512)
    {
        PANIC("Too much entries in memory map!");
    }
    
    for (uint32_t j = _num_entries; j > i; --j)
    {
        _entries[j] = _entries[j - 1];
    }
    
    ++_num_entries;
}

void memory::map::_shrink(uint32_t i)
{
    if (i >= _num_entries)
    {
        PANIC("Insane argument for _shrink.");
    }
    
    if (_num_entries == 0)
    {
        PANIC("Tried to shrink empty memory map!");
    }
    
    for (; i < _num_entries - 1; ++i)
    {
        _entries[i] = _entries[i + 1];
    }
    
    --_num_entries;
}

void memory::map::add_entry(memory::map_entry & entry)
{
    if (entry.length == 0)
    {
        return;
    }
    
    if (_num_entries == 0)
    {
        _entries[0] = entry;
    
        ++_num_entries;
        
        return;
    }

    for (uint32_t i = 0; i < _num_entries; ++i)
    {
        if (entry.type == _entries[i].type)
        {
            if (entry.base >= _entries[i].base && entry.base + entry.length <= _entries[i].base + _entries[i].length)
            {
                return;
            }
            
            if (_entries[i].base + _entries[i].length >= entry.base && _entries[i].base + _entries[i].length <= entry.base + entry.length)
            {
                map_entry tmp = entry;
                entry = _entries[i];
                _entries[i] = tmp;
            }
            
            if (entry.base + entry.length >= _entries[i].base && entry.base + entry.length <= _entries[i].base + _entries[i].length)
            {
                _entries[i].length = _entries[i].base + _entries[i].length - entry.base;
                _entries[i].base = entry.base;
                
                return;
            }
        }
        
        if (entry.base < _entries[i].base && entry.base + entry.length > _entries[i].base + _entries[i].length)
        {
            map_entry tmp;
            tmp = entry;
            entry = _entries[i];
            _entries[i] = tmp;
        }
        
        if (entry.base >= _entries[i].base && entry.base + entry.length <= _entries[i].base + _entries[i].length)
        {
            if (_entries[i].type > entry.type)
            {
                return;
            }
            
            if (entry.base > _entries[i].base)
            {
                map_entry other{}, another{};
                other.base = entry.base + entry.length;
                other.length = _entries[i].base + _entries[i].length - other.base;
                
                another.base = _entries[i].base;
                another.length = _entries[i].base + _entries[i].length - entry.base;
                
                _shrink(i);
                
                add_entry(other);
                add_entry(another);
                add_entry(entry);
                
                return;
            }
        }
        
        if (_entries[i].base < entry.base && _entries[i].base + _entries[i].length > entry.base && _entries[i].base + 
            _entries[i].length < entry.base + entry.length)
        {
            map_entry tmp;
            tmp = entry;
            entry = _entries[i];
            _entries[i] = tmp;
        }
        
        if (entry.base < _entries[i].base && entry.base + entry.length > _entries[i].base && entry.base + entry.length < 
            _entries[i].base + _entries[i].length)
        {                
            if (entry.type > _entries[i].type)
            {
                uint64_t old = _entries[i].base;
                _entries[i].base = entry.base + entry.length;
                _entries[i].length -= old - _entries[i].base;
                
                add_entry(entry);
                
                return;
            }
            
            entry.length = _entries[i].base - entry.base;
            
            add_entry(entry);
            
            return;
        }
    }
    
    for (uint32_t i = 0; i < _num_entries; ++i)
    {
        if (i == 0)
        {
            if (entry.base < _entries[i].base)
            {
                _expand(0);
                
                _entries[i] = entry;
                
                return;
            }
        }
        
        else
        {
            if (_entries[i - 1].base < entry.base && entry.base < _entries[i].base)
            {
                _expand(i);
                
                _entries[i] = entry;
                
                return;
            }
        }
    }

    _entries[_num_entries++] = entry;
}

uint32_t memory::map::find_last_usable(uint32_t size)
{
    size += 4096 - size % 4096;
    
    for (uint32_t i = _num_entries; i != (uint32_t)-1; --i)
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

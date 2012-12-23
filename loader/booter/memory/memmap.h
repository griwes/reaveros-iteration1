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

#pragma once

#include <cstdint>
#include <cstddef>

#include <screen/screen.h>

namespace memory
{
    class map_entry
    {
    public:
        map_entry() : base(0), length(0), type(0), extended_attribs(0)
        {
        }
        
        map_entry(map_entry * old) : base(old->base), length(old->length), type(old->type), 
            extended_attribs(old->extended_attribs)
        {
        }
        
        uint64_t base;
        uint64_t length;
        uint32_t type;
        uint32_t extended_attribs;
    } __attribute__((packed));
    
    void print_map();
    
    class map
    {
    public:
        friend void memory::print_map();
        
        static void add_entry(map_entry *);
        
        static bool usable(uint64_t);
        static uint64_t next_usable(uint64_t);
        
        static uint32_t find_last_usable(uint32_t);
        
    private:
        static map_entry _entries[512]; // more = insanity
        
        static uint32_t _num_entries;
        
        static void _combine_entries(uint64_t, uint64_t);
        static void _merge_siblings(uint64_t);
    };
}

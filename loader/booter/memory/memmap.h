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

#include "../processor/processor.h"
#include "../screen/screen.h"

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
    
    class chained_map_entry : public map_entry
    {
    public:
        chained_map_entry() : map_entry(), proximity_domain(-1), prev(nullptr), next(nullptr)
        {
        }
        
        chained_map_entry(map_entry * old) : map_entry(old), proximity_domain(-1), prev(nullptr), next(nullptr)
        {
        }
        
        uint32_t proximity_domain;
        
        chained_map_entry * prev;
        chained_map_entry * next;
    } __attribute__((packed));
    
    class map
    {
    public:
        template<typename T>
        friend void screen::print(const T &);
        
        map();
        map(map_entry *, uint32_t);
        ~map();
        
        map * sanitize();
        void add_entry(chained_map_entry *);
        void apply_numa(processor::numa_env *);
        
        bool usable(uint64_t, uint32_t = -1);
        uint64_t next_usable(uint64_t, uint32_t = -1);
        
        map_entry ** split_numa_memmaps();
        
    private:
        map_entry * _sequence_entries;
        chained_map_entry * _entries;
        uint32_t _num_entries;
    };    
}

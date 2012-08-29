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

#include "memmap.h"

namespace memory
{
    namespace manager
    {
        class allocator
        {
        public:
            allocator() {}
            virtual ~allocator() {}
            
            virtual void * allocate(uint32_t) = 0;
            virtual void deallocate(void *) = 0;
        };
        
        class placement_allocator : public allocator
        {
        public:
            placement_allocator(uint32_t, map &);
            virtual ~placement_allocator();
            
            virtual void * allocate(uint32_t);
            virtual void deallocate(void *);
            
        private:
            map & memory_map;
            uint32_t placement_address;
        };
        
        allocator * make_placement_allocator(uint32_t, map &);
    }
}
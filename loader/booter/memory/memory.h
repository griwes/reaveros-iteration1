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

#include "../processor/processor.h"

namespace memory
{    
    namespace manager
    {
        class allocator;
    }
    
    extern manager::allocator * default_allocator;
    
    void initialize(uint32_t, map &);
    void init_protected_paging();
    void init_pat();
    void prepare_long_mode();
    
    template<typename T>
    void zero(T * ptr, uint32_t size = 1)
    {
        while (size-- > 0)
        {
            *ptr++ = {};
        }
    }
    
    template<typename T>
    void copy(T * src, T * dest, uint32_t size = 1)
    {
        while (size-- > 0)
        {
            *dest++ = *src++;
        }
    }
    
    inline void zero(uint8_t * ptr, uint32_t size)
    {
        zero((uint32_t *)ptr, size / 4);
        
        size %= 4;
        
        while (size-- > 0)
        {
            *ptr++ = {};
        }
    }
    
    inline void copy(uint8_t * src, uint8_t * dest, uint32_t size)
    {
        copy((uint32_t *)src, (uint32_t *)dest, size / 4);
        
        size %= 4;
        
        while (size-- > 0)
        {
            *dest++ = *src++;
        }
    }
}

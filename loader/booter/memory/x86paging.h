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

namespace memory
{
    namespace x86
    {
        enum cache_options
        {
            uncacheable = 0,
            write_combining = 1,
            write_through = 2,
            write_protected = 3,
            write_back = 4,
            uncached = 5
        };
        
        struct page_table_entry
        {
            uint32_t present:1;
            uint32_t writeable:1;
            uint32_t user:1;
            uint32_t pwt:1;
            uint32_t pcd:1;
            uint32_t accessed:1;
            uint32_t dirty:1;
            uint32_t pat:1;
            uint32_t global:1;
            uint32_t ignored:3;
            uint32_t address:32 - 12;
        };
        
        inline void set_cache(page_table_entry & pte, cache_options option = write_back)
        {
            pte.pat = (option & 4) >> 2;
            pte.pcd = (option & 2) >> 1;
            pte.pwt = option & 1;
        }
        
        struct page_table
        {
            page_table(uint32_t id)
            {
                for (uint32_t i = 0; i < 1024; ++i)
                {
                    entries[i] = {};
                    entries[i].present = 1;
                    entries[i].address = (id * 4 * 1024 * 1024 + i * 4 * 1024) >> 12;
                    
                    set_cache(entries[i]);
                }
            }
            
            page_table_entry entries[1024];
        };
        
        struct page_directory_entry
        {
            page_directory_entry & operator=(page_table * pt)
            {
                present = 1;
                writeable = 1;
                
                address = (uint32_t)pt >> 12;
                
                return *this;
            }
            
            uint32_t present:1;
            uint32_t writeable:1;
            uint32_t user:1;
            uint32_t pwt:1;
            uint32_t pcd:1;
            uint32_t accessed:1;
            uint32_t ignored:6;
            uint32_t address:32 - 12;
        };
        
        struct page_directory
        {
            page_directory()
            {
                for (uint32_t i = 0; i < 1024; ++i)
                {
                    entries[i] = {};
                }
            }
            
            page_directory_entry entries[1024];
        };
        
        inline void set_cache(uint32_t address, cache_options option)
        {
            page_table * pt = (page_table *)(0xFFC00000 + 0x400 * (address >> 22));
            page_table_entry & pte = pt->entries[address >> 12 & 0x3FF];
            
            set_cache(pte, option);
            
            invlpg(address);
        }
    }
}
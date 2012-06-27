/**
 * ReaverOS
 * kernel/memory/vmm.h
 * Virtual memory manager declaration.
 */

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

#ifndef __rose_kernel_memory_vmm_h__
#define __rose_kernel_memory_vmm_h__

#include "../types.h"
#include "memory.h"
#include "vm.h"
#include "../lib/stack.h"

namespace Memory
{
    namespace VMM
    {
        void * AllocPagingPages(uint64);

        inline void MapPage(uint64 p)
        {
            Memory::CurrentVAS->MapPage(p);
        }
        
        inline void MapPage(uint64 p, uint64 b)
        {
            Memory::CurrentVAS->MapPage(p, b);
        }
        
        inline void MapPages(uint64 s, uint64 e)
        {
            Memory::CurrentVAS->MapPages(s, e);
        }
        
        inline void MapPages(uint64 s, uint64 l, uint64 b, bool cache = false)
        {
            Memory::CurrentVAS->MapPages(s, l, b, cache);
        }
        
        inline void UnmapPage(uint64 p)
        {
            Memory::CurrentVAS->UnmapPage(p);
        }
        
        inline void UnmapPages(uint64 s, uint64 e)
        {
            Memory::CurrentVAS->UnmapPages(s, e);
        }

        inline void PushPage(uint64 p)
        {
            if (Memory::CorePages)
            {
                Memory::CorePages->Push(p);
            }

            else
            {
                Memory::GlobalPages->Push(p);
            }
        }

        extern bool Ready;
    }
}

#endif
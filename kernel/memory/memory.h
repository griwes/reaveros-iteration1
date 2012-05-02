/**
 * ReaverOS
 * kernel/memory/memory.h
 * Main memory subsystem header.
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

#ifndef _rose_kernel_memory_memory_h_
#define _rose_kernel_memory_memory_h_

#include "../types.h"
#include "memorymap.h"
#include "heap.h"
#include "paging.h"

namespace Memory
{
    void PreInitialize(void *);
    void Initialize(Memory::MemoryMapEntry *, uint32);

    void RemapKernel();
    void CreateFreePageStack();

    void AlignPlacementToPage();
    void * AlignToNextPage(uint64);

    template<typename T>
    void Zero(T * p)
    {
        uint32 iSize = sizeof(T);
        
        for (uint32 i = 0; i < iSize; i++)
        {
            *(char *)(p++) = 0;
        }
    }
    
    template<typename T>
    void Zero(T * p, uint32 iCount)
    {
        uint32 iSize = sizeof(T) * iCount;
        
        for (uint32 i = 0; i < iSize; i++)
        {
            *(char *)(p++) = 0;
        }
    }

    extern void * pPlacementAddress;
    extern Memory::MemoryMap * pMemoryMap;
    extern Memory::Heap * KernelHeap;
    extern Paging::PML4 * KernelPML4;
    extern Paging::PageDirectory * KernelSpace[2];
}

#endif
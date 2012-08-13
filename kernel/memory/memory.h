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

#ifndef __rose_kernel_memory_memory_h__
#define __rose_kernel_memory_memory_h__

#include "../types.h"

namespace Lib
{
    class Stack;
}

namespace Paging
{
    struct PML4;
    struct PageDirectory;
}

namespace Memory
{
    namespace VM
    {
        class Region;
        class AddressSpace;
    }

    class MemoryMapEntry;
    class MemoryMap;
    class Heap;
    
    void PreInitialize(void *);
    void Initialize(Memory::MemoryMapEntry *, uint32);
    void InitializeRegions();
    
    void RemapKernel();

    void AlignPlacementToPage();
    void * AlignToNextPage(uint64);

    template<typename T>
    void Zero(T * ptr, uint32 iCount = 1)
    {
        uint32 iSize = sizeof(T) * iCount;
        uint8 * p = (uint8 *)ptr;

        for (uint64 i = 0; i < iSize; i++)
        {
            *(p++) = 0;
        }
    }

    template<typename T>
    void Copy(T * src, T * dest, uint32 iCount = 1)
    {
        uint32 iSize = sizeof(T) * iCount;
        
        uint8 * ps = (uint8 *)src;
        uint8 * pd = (uint8 *)dest;

        for (uint64 i = 0; i < iSize; i++)
        {
            *(pd++) = *(ps++);
        }
   }

    extern void * PlacementAddress;
    extern Memory::MemoryMap * SystemMemoryMap;
    extern Memory::Heap * KernelHeap;
    extern uint64 StackStart;
    extern Lib::Stack * GlobalPages;
    extern Lib::Stack * CorePages;
    extern Lib::Stack * PagingStructures;
    extern Lib::Stack * VMPages;
    extern Lib::Stack * VMRegions;
    extern Lib::Stack * VMAddressSpaces;

    extern uint64 PagingStructuresCount;
    const uint64 PagingStructuresMax = 1025ull * 1024 * 256;
    extern uint64 VMPagesCount;
    const uint64 VMPagesMax = 1025ull * 1024 * 256;
    extern uint64 VMRegionsCount;
    const uint64 VMRegionsMax = 1024ull * 1024ull;
    extern uint64 VMAddressSpacesCount;
    const uint64 VMAddressSpacesMax = 1024ull * 64;
    
    extern VM::Region * KernelRegion;
    extern VM::Region * VideoBackbufferRegion;
    extern VM::Region * VideoMemoryRegion;
    extern VM::Region * KernelHeapRegion;
    extern VM::Region * PagingStructuresPoolRegion;
    extern VM::Region * PagingStructuresPoolStackRegion;
    extern VM::Region * GlobalPageStackRegion;
    extern VM::Region * VMPagePoolRegion;
    extern VM::Region * VMPagePoolStackRegion;
    extern VM::Region * VMRegionPoolRegion;
    extern VM::Region * VMRegionPoolStackRegion;
    extern VM::Region * VMAddressSpacePoolRegion;
    extern VM::Region * VMAddressSpacePoolStackRegion;
    extern VM::Region * CoreDataRegion;
    extern VM::Region * CorePageStackRegion;
    extern VM::AddressSpace * CurrentVAS;
    extern uint64 AvailableMemory;
}

#endif
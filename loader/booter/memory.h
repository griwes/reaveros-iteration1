/**
 * ReaverOS
 * loader/booter/memory.h
 * Memory-related routines.
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

#ifndef __rose_loader_booter_memory_h__
#define __rose_loader_booter_memory_h__

#include "types.h"

class MemoryMapEntry
{
public:
    uint64 Base;
    uint64 Length;
    uint32 type;
    uint32 ACPI30;

    const char * Type();
} __attribute__((packed));

namespace Memory
{
    void Initialize(void *);
    void PrintMemoryMap(MemoryMapEntry *, uint32);
    uint64 AlignToNextPage(uint64);

    void * Place(uint32);
    void * PlacePageAligned(uint32);

    void Zero(char *, uint32);

    void Map(uint64, uint64, uint64 &, bool = false);
    uint64 CountPagingStructures(uint64, uint64);

    void UpdateMemoryMap(uint64, uint64);
    
    uint64 Copy(uint32, uint32, uint64);
    uint64 CreateMemoryMap(MemoryMapEntry *, uint32, uint64);
    
    extern void * pPlacement;
    extern uint64 iFirstFreePageAddress;
}

#endif
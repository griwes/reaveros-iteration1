/**
 * ReaverOS
 * loader/booter/memory.cpp
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

#include "memory.h"
#include "screen.h"

using Screen::bout;
using Screen::nl;

void * Memory::pPlacement;

const char * MemoryMapEntry::Type()
{
    switch (this->type)
    {
        case 1:
            return "Free memory (1)";
        case 2:
            return "Reserved memory (2)";
        case 3:
            return "ACPI reclaimable memory (3)";
        case 4:
            return "ACPI NVS memory (4)";
        case 5:
            return "Bad memory (5)";
    }

    return "?!";
}

void Memory::Initialize(void * pAddr)
{
    Memory::pPlacement = pAddr;
}

void * Memory::AlignToNextPage(void * pAddr)
{
    uint32 addr = (uint32)pAddr;
    addr += 4095;
    addr &= ~(uint32)4095;
    return (void *)addr;
}

void Memory::PrintMemoryMap(MemoryMapEntry * pMap, uint32 count)
{
    bout->Hex();
    
    *bout << "Base address       | Length             | Type" << nl;
    *bout << "-------------------|--------------------|----------------------------" << nl;

    uint64 iTotalMemory = 0;
    
    for (uint32 i = 0; i < count; i++)
    {
        if (pMap->type == 1)
        {
            iTotalMemory += pMap->Length;
        }

        *bout << "0x";
        
        for (uint32 j = 60; j >= 4; j -= 4)
        {
            if ((pMap->Base & ((uint64)0xf << j)) != 0)
            {
                break;
            }

            *bout << "0";
        }
        
        *bout << pMap->Base << " | 0x";

        for (uint32 j = 60; j >= 4; j -= 4)
        {
            if ((pMap->Length & ((uint64)0xf << j)) != 0)
            {
                break;
            }

            *bout << "0";
        }

        *bout << pMap->Length << " | " << pMap->Type() << nl;
        *bout << "-------------------|--------------------|----------------------------" << nl;
        pMap++;
    }

    bout->Dec();

    *bout << "Total memory available: " << iTotalMemory / (1024 * 1024 * 1024) << " GiB " <<
                (iTotalMemory % (1024 * 1024 * 1024)) / (1024 * 1024) << " MiB " <<
                (iTotalMemory % (1024 * 1024)) / 1024 << " KiB" << nl;
}

void * Memory::Place(uint32 size)
{
    void * ret = Memory::pPlacement;
    Memory::pPlacement += size;
    return ret;
}

void * Memory::PlacePageAligned(uint32 size)
{
    Memory::pPlacement += 4095;
    uint32 _ = (uint32)Memory::pPlacement;
    _ &= ~(uint32)4095;
    Memory::pPlacement = (void *)_;
    
    return Memory::Place(size);
}

void Memory::Zero(char * buf, uint32 size)
{
    while (size--)
    {
        *buf++ = 0;
    }
}

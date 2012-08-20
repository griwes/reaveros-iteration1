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
#include "paging.h"
#include "processor.h"

using Screen::bout;
using Screen::nl;

extern "C"
{
    void _copy(uint32, uint32, uint32);
}

void * Memory::pPlacement;
uint64 Memory::iFirstFreePageAddress;
uint64 Memory::TotalMemory;

const char * MemoryMapEntry::Type()
{
    switch (type)
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
    Memory::iFirstFreePageAddress = 64 * 1024 * 1024;
}

uint64 Memory::AlignToNextPage(uint64 pAddr)
{
    pAddr += 4095;
    pAddr &= ~(uint64)4095;
    return pAddr;
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
        pMap++;
    }

    bout->Dec();

    *bout << "-------------------|--------------------|----------------------------" << nl;
    *bout << "Total memory available: " << iTotalMemory / (1024 * 1024 * 1024) << " GiB " <<
                (iTotalMemory % (1024 * 1024 * 1024)) / (1024 * 1024) << " MiB " <<
                (iTotalMemory % (1024 * 1024)) / 1024 << " KiB" << nl << nl;

    Memory::TotalMemory = iTotalMemory;
}

void * Memory::Place(uint32 size)
{
    void * ret = Memory::pPlacement;
    uint32 _ = (uint32)Memory::pPlacement;
    _ += size;
    Memory::pPlacement = (void *)_;
    
    return ret;
}

void * Memory::PlacePageAligned(uint32 size)
{
    uint32 _ = (uint32)Memory::pPlacement;
    _ += 4095;
    _ &= ~(uint32)4095;
    Memory::pPlacement = (void *)_;
    
    return Memory::Place(size);
}

void Memory::Zero(char * buf, uint32 size)
{
    while (size)
    {
        *buf++ = 0;
        size--;
    }
}

uint64 Memory::Copy(uint32 pSource, uint32 iSize, uint64 pDestination)
{
    uint64 p = Memory::iFirstFreePageAddress;

    iSize += (iSize % 4);
    if (iSize < 4 * 1024)
    {
        iSize = 4 * 1024;
    }
    
    Memory::Map(pDestination, pDestination + iSize, Memory::iFirstFreePageAddress);
    Memory::Map(0x8000000, 0x8000000 + iSize, p);
    asm volatile ("invlpg (0x8000000)" ::: "memory");
    
    iSize /= 4;
    _copy(pSource, 0x8000000, iSize);
    
    return pDestination + iSize * 4;
}

void Memory::Map(uint64 pBegin, uint64 pEnd, uint64 & pPhysicalStart, bool bCacheDisable)
{
    PML4 * pml4 = Processor::PagingStructures;

    pEnd = Memory::AlignToNextPage(pEnd);

    uint64 startpml4e = (pBegin >> 39) & 511;
    uint64 startpdpte = (pBegin >> 30) & 511;
    uint64 startpde = (pBegin >> 21) & 511;
    uint64 startpte = (pBegin >> 12) & 511;
    
    uint64 endpml4e = (pEnd >> 39) & 511;
    uint64 endpdpte = (pEnd >> 30) & 511;
    uint64 endpde = (pEnd >> 21) & 511;
    uint64 endpte = (pEnd >> 12) & 511;
    
    while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte))
    {        
        PageDirectoryPointerTable * pdpt;
        
        if (pml4->Entries[startpml4e].Present == 1)
        {
            pml4->Entries[startpml4e].CacheDisable = bCacheDisable;
            pdpt = pml4->PointerTables[startpml4e];
        }
        
        else
        {
            pdpt = (PageDirectoryPointerTable *)Memory::PlacePageAligned(
                sizeof(PageDirectoryPointerTable));
            
            Memory::Zero((char *)pdpt, sizeof(PageDirectoryPointerTable));
            
            pml4->Entries[startpml4e].Present = 1;
            pml4->Entries[startpml4e].ReadWrite = 1;
            pml4->Entries[startpml4e].CacheDisable = bCacheDisable;
            pml4->Entries[startpml4e].PDPTAddress = (uint32)pdpt >> 12;
            
            pml4->PointerTables[startpml4e] = pdpt;
        }
        
        while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
            && startpdpte < 512)
        {
            PageDirectory * pd;
            
            if (pdpt->Entries[startpdpte].Present == 1)
            {
                pdpt->Entries[startpdpte].CacheDisable = bCacheDisable;
                pd = pdpt->PageDirectories[startpdpte];
            }
            
            else
            {
                pd = (PageDirectory *)Memory::PlacePageAligned(sizeof(PageDirectory));
            
                Memory::Zero((char *)pd, sizeof(PageDirectory));
                
                pdpt->Entries[startpdpte].Present = 1;
                pdpt->Entries[startpdpte].ReadWrite = 1;
                pdpt->Entries[startpdpte].CacheDisable = bCacheDisable;
                pdpt->Entries[startpdpte].PageDirectoryAddress = (uint32)pd >> 12;
                
                pdpt->PageDirectories[startpdpte] = pd;
            }
            
            while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
                && startpde < 512)
            {
                PageTable * pt;
                
                if (pd->Entries[startpde].Present == 1)
                {
                    pd->Entries[startpde].CacheDisable = bCacheDisable;
                    pt = pd->PageTables[startpde];
                }
                
                else
                {
                    pt = (PageTable *)Memory::PlacePageAligned(sizeof(PageTable));
                    
                    Memory::Zero((char *)pt, sizeof(PageTable));
                    
                    pd->Entries[startpde].Present = 1;
                    pd->Entries[startpde].ReadWrite = 1;
                    pd->Entries[startpde].CacheDisable = bCacheDisable;
                    pd->Entries[startpde].PageTableAddress = (uint32)pt >> 12;
                    
                    pd->PageTables[startpde] = pt;
                }
                
                while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
                    && startpte < 512)
                {
                    pt->Entries[startpte].Present = 1;
                    pt->Entries[startpte].ReadWrite = 1;
                    pt->Entries[startpte].CacheDisable = bCacheDisable;
                    
                    uint64 addr = pPhysicalStart;
                    pt->Entries[startpte].PageAddress = addr >> 12;
                    
                    startpte++;

                    pPhysicalStart += 4096;
                }
                
                if (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte))
                {
                    startpde++;
                    startpte = 0;
                }
                
                else
                {
                    return;
                }
            }
            
            if (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte))
            {
                startpde = 0;
                startpdpte++;
            }
            
            else
            {
                return;
            }
        }
        
        if (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte))
        {
            startpdpte = 0;
            startpml4e++;
        }
        
        else
        {
            return;
        }
    }
}

uint64 Memory::CreateMemoryMap(MemoryMapEntry * pMemoryMap, uint32 iCount, uint64 pDestinationAddress)
{
    uint64 p = Memory::iFirstFreePageAddress;
    Memory::Map(pDestinationAddress, pDestinationAddress + sizeof(MemoryMapEntry) * (iCount + 1),
                Memory::iFirstFreePageAddress);
    Memory::Map(0x8000000, 0x8000000 + sizeof(MemoryMapEntry) * (iCount + 1), p);
    
    asm volatile ("invlpg (0x8000000)" ::: "memory");

    MemoryMapEntry * pKernelMemoryMap = (MemoryMapEntry *)0x8000000;
    
    for (uint32 i = 0; i < iCount; i++)
    {
        pKernelMemoryMap->type = pMemoryMap[i].type;
        pKernelMemoryMap->Base = pMemoryMap[i].Base;
        pKernelMemoryMap->Length = pMemoryMap[i].Length;
        pKernelMemoryMap->ACPI30 = pMemoryMap[i].ACPI30;
        pKernelMemoryMap++;
    }

    pKernelMemoryMap->type = 0xffff;
    pKernelMemoryMap->Base = 64 * 1024 * 1024;
    pKernelMemoryMap->Length = 0;
    pKernelMemoryMap->ACPI30 = 0;

    return Memory::AlignToNextPage(pDestinationAddress + sizeof(MemoryMapEntry) * (iCount + 1));
}

// that /= 350 was roughly estimated
uint64 Memory::CountPagingStructures(uint64 pBegin, uint64 pEnd)
{
    pEnd = Memory::AlignToNextPage(pEnd);
    
    uint64 iSize = pEnd - pBegin;
    iSize /= 350;
    iSize = Memory::AlignToNextPage(iSize);
    
    return iSize;
}

void Memory::UpdateMemoryMap(uint64 size)
{
    MemoryMapEntry * p = (MemoryMapEntry *)0x8000000;

    while (p->type != 0xffff)
    {
        p++;
    }

    p->Length = size;
    
    return;
}

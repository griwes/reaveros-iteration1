/**
 * ReaverOS
 * kernel/memory/paging.cpp
 * Mapping functions implementation.
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

#include "paging.h"
#include "memory.h"

void Paging::PML4::Map(uint64 pBaseVirtual, uint64 iLength, uint64 pBasePhysical, bool bCacheDisable,
                        bool bReadWrite, bool bUser, bool bWriteThrough)
{
    uint64 pEnd = pBaseVirtual + iLength;
    
    uint64 startpml4e = (pBaseVirtual >> 39) & 511;
    uint64 startpdpte = (pBaseVirtual >> 30) & 511;
    uint64 startpde = (pBaseVirtual >> 21) & 511;
    uint64 startpte = (pBaseVirtual >> 12) & 511;
    
    uint64 endpml4e = (pEnd >> 39) & 511;
    uint64 endpdpte = (pEnd >> 30) & 511;
    uint64 endpde = (pEnd >> 21) & 511;
    uint64 endpte = (pEnd >> 12) & 511;
    
    while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte))
    {
        PageDirectoryPointerTable * pdpt;
        
        if (this->Entries[startpml4e].Present == 1)
        {
            pdpt = this->PointerTables[startpml4e];
        }
        
        else
        {
            pdpt = new PageDirectoryPointerTable;
            
            Memory::Zero((char *)pdpt, sizeof(PageDirectoryPointerTable));
            
            this->Entries[startpml4e].Present = 1;
            this->Entries[startpml4e].PDPTAddress = (uint64)pdpt >> 12;
            
            this->PointerTables[startpml4e] = pdpt;
        }
        
        while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
            && startpdpte < 512)
        {
            PageDirectory * pd;
            
            if (pdpt->Entries[startpdpte].Present == 1)
            {
                pd = pdpt->PageDirectories[startpdpte];
            }
            
            else
            {
                pd = new PageDirectory;
                
                Memory::Zero((char *)pd, sizeof(PageDirectory));
                
                pdpt->Entries[startpdpte].Present = 1;
                pdpt->Entries[startpdpte].PageDirectoryAddress = (uint64)pd >> 12;
                
                pdpt->PageDirectories[startpdpte] = pd;
            }
            
            while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
                && startpde < 512)
            {
                PageTable * pt;
                
                if (pd->Entries[startpde].Present == 1)
                {
                    pt = pd->PageTables[startpde];
                }
                
                else
                {
                    pt = new PageTable;
                    
                    Memory::Zero((char *)pt, sizeof(PageTable));
                    
                    pd->Entries[startpde].Present = 1;
                    pd->Entries[startpde].PageTableAddress = (uint64)pt >> 12;
                    
                    pd->PageTables[startpde] = pt;
                }
                
                while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
                    && startpte < 512)
                {
                    pt->Entries[startpte].Present = 1;
                    pt->Entries[startpte].ReadWrite = bReadWrite;
                    pt->Entries[startpte].CacheDisable = bCacheDisable;
                    pt->Entries[startpte].User = bUser;
                    pt->Entries[startpte].WriteThrough = bWriteThrough;
                    pt->Entries[startpte].CacheDisable = bCacheDisable;
                    
                    uint64 addr = pBasePhysical;
                    pt->Entries[startpte].PageAddress = addr >> 12;
                    
                    startpte++;
                    
                    pBasePhysical += 4096;
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

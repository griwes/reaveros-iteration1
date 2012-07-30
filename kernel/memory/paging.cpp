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
#include "heap.h"
#include "vmm.h"

uint64 Paging::PML4::s_iBase = 0;

Paging::PML4::PML4(uint64 iBase)
{
    Memory::Zero(&this->Entries, 512);
    Memory::Zero(&this->PointerTables, 512);

    s_iBase = iBase;
}

void Paging::PML4::Map(uint64 pBaseVirtual, uint64 iLength, uint64 pBasePhysical, bool bCacheDisable,
                        bool bReadWrite, bool bUser, bool bWriteThrough, PML4 * old)
{
    if (old == (PML4 *)0)
    {
        old = this;
    }

    uint64 pEnd = pBaseVirtual + iLength;
    pEnd = (pEnd + 4095) & ~(uint64)4095;
    
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

            Memory::Zero(pdpt);
            
            this->Entries[startpml4e].Present = 1;
            this->Entries[startpml4e].PDPTAddress = old->GetPhysicalAddress((uint64)pdpt) >> 12;
            
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

                Memory::Zero(pd);
                
                pdpt->Entries[startpdpte].Present = 1;
                pdpt->Entries[startpdpte].PageDirectoryAddress = old->GetPhysicalAddress((uint64)pd) >> 12;
                
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
                    
                    Memory::Zero(pt);

                    pd->Entries[startpde].Present = 1;
                    pd->Entries[startpde].PageTableAddress = old->GetPhysicalAddress((uint64)pt) >> 12;
                    
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

                    if (pt->Entries[startpte].Present && Memory::PlacementAddress == 0)
                    {
                        PANIC("Tried to remap already mapped page!");
                    }

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

uint64 Paging::PML4::ArePSAvailable(uint64 address)
{
    auto a = PointerTables[(address >> 39) & 511];

    if (a)
    {
        auto b = a->PageDirectories[(address >> 30) & 511];
    
        if (b)
        {
            auto c = b->PageTables[(address >> 21) & 511];
        
            if (c)
            {
                return 3;
            }
            
            return 2;
        }
        
        return 1;
    }
    
    return 0;
}

// use with caution
void Paging::PML4::SetPS(uint64 address, uint64 ipdpt, uint64 ipd, uint64 ipt)
{
    auto a = PointerTables[(address >> 39) & 511];
    auto b = a->PageDirectories[(address >> 30) & 511];

    PageDirectoryPointerTable * pdpt = (PageDirectoryPointerTable *)ipdpt;
    
    PointerTables[(address >> 39) & 511] = pdpt;
    Entries[(address >> 39) & 511].PDPTAddress = ipdpt ? GetPhysicalAddress(ipdpt) >> 12 : 0;
    Entries[(address >> 39) & 511].Present = 1;
    
    PageDirectory * pd = (PageDirectory *)ipd;
        
    a->PageDirectories[(address >> 30) & 511] = pd;
    a->Entries[(address >> 30) & 511].PageDirectoryAddress = ipd ? GetPhysicalAddress(ipd) >> 12 : 0;
    a->Entries[(address >> 30) & 511].Present = 1;

    PageTable * pt = (PageTable *)ipt;
            
    b->PageTables[(address >> 21) & 511] = pt;
    b->Entries[(address >> 21) & 511].PageTableAddress = ipt ? GetPhysicalAddress(ipt) >> 12 : 0;
    b->Entries[(address >> 21) & 511].Present = 1;

    Paging::Invlpg(address);
}

uint64 Paging::PML4::Unmap(uint64 pAddr)
{
    uint64 physical = this->GetPhysicalAddress(pAddr);
    Memory::Zero(&this->PointerTables[(pAddr >> 39) & 511]->PageDirectories[(pAddr >> 30) & 511]->PageTables[(pAddr >> 21) & 511]
                    ->Entries[(pAddr >> 39) & 511]);
    return physical;
}

void Paging::Invlpg(uint64 addr)
{
    asm volatile ("invlpg (%0)" :: "r"(addr) : "memory");
}

static void * _alloc(uint64 iSize)
{
    if (Memory::PlacementAddress)
    {
        Memory::AlignPlacementToPage();
        void * ret = Memory::PlacementAddress;
        uint64 _ = (uint64)Memory::PlacementAddress;
        _ += iSize;
        Memory::PlacementAddress = (void *)_;
        return ret;
    }
        
    return Memory::VMM::AllocPagingPages();
}

void * Paging::PageTable::operator new(uint64 iSize)
{
    return _alloc(iSize);
}

void * Paging::PageTable::operator new[](uint64 iSize)
{
    return _alloc(iSize);
}

void * Paging::PageDirectory::operator new(uint64 iSize)
{
    return _alloc(iSize);
}

void * Paging::PageDirectory::operator new[](uint64 iSize)
{
    return _alloc(iSize);
}

void * Paging::PageDirectoryPointerTable::operator new(uint64 iSize)
{
    return _alloc(iSize);
}

void * Paging::PageDirectoryPointerTable::operator new[](uint64 iSize)
{
    return _alloc(iSize);
}

void * Paging::PML4::operator new(uint64 iSize)
{
    return _alloc(iSize);
}

void * Paging::PML4::operator new[](uint64 iSize)
{
    return _alloc(iSize);
}
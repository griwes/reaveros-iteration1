/**
 * ReaverOS
 * kernel/memory/vm.cpp
 * Virtual memory utility functions.
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

#include "vm.h"
#include "../lib/stack.h"
#include "../scheduler/scheduler.h"

Memory::VM::AddressSpace::AddressSpace(uint64 base)
    : m_pPML4(new Paging::PML4(base))
{
}

Memory::VM::AddressSpace::~AddressSpace()
{
    for (auto it = m_mRegions.Begin(), end = m_mRegions.End(); it != end; ++it)
    {
        delete it.Value();
    }
}

void Memory::VM::AddressSpace::AddRegion(uint64 base, uint64 end, Region * region)
{
    m_lock.Lock();

    for (auto it = m_mRegions.Get(base), en = m_mRegions.Get(end); it != en; ++it)
    {
        PANIC("Tried to add overlapping region.");
    }

    if (!region)
    {
        region = new Region;
        region->Base = base;
        region->End = end;
    }

    m_mRegions.Insert(base, end - 1, region);

    m_lock.Unlock();
}

void Memory::VM::AddressSpace::AddRegion(Memory::VM::Region * pRegion)
{
    AddRegion(pRegion->Base, pRegion->End, pRegion);
}

void Memory::VM::AddressSpace::RemoveRegion(uint64 base)
{
    m_lock.Lock();

    if (auto it = m_mRegions.Get(base))
    {
        if (!m_mRegions.Remove(it))
        {
            PANIC("Failed to remove region from regions range map.");
        }
    }

    else
    {
        PANIC("Tried to remove unexistent region from regions range map.");
    }

    m_lock.Unlock();
}

void Memory::VM::AddressSpace::RemoveRegion(Memory::VM::Region * pRegion)
{
    m_lock.Lock();

    if (auto it = m_mRegions.Get(pRegion->Base))
    {
        if (it.Value() != pRegion)
        {
            PANIC("Region mismatch - tried to remove invalid region from address space.");
        }

        if (!m_mRegions.Remove(it))
        {
            PANIC("Failed to remove region from range map.");
        }
    }

    else
    {
        PANIC("Tried to remove unexistent region from regions range map.");
    }

    m_lock.Unlock();

    RemoveRegion(pRegion->Base);
}

void Memory::VM::AddressSpace::MapPage(uint64 address)
{
    if (!Memory::PlacementAddress)
    {
        Page * pPage = new Page;
        pPage->VirtualAddress = address;
        pPage->PhysicalAddress = (CorePages ? Memory::CorePages->Pop() : Memory::GlobalPages->Pop());

        MapPage(pPage);
    }

    else
    {
        m_pPML4->Map(address, 4096, (CorePages ? Memory::CorePages->Pop() : Memory::GlobalPages->Pop()));
    }
}

void Memory::VM::AddressSpace::MapPage(Memory::VM::Page * pPage)
{
    m_lock.Lock();
    
    if (Lib::RangeMap<Region *>::Iterator it = m_mRegions.Get(pPage->VirtualAddress))
    {
        it.Value()->AddPage(pPage);
    }

    else
    {
        PANIC("Tried to map page outside any region.");
    }

    m_lock.Unlock();
}

void Memory::VM::AddressSpace::UnmapPage(uint64 address)
{
    m_lock.Lock();

    if (auto it = m_mRegions.Get(address))
    {
        m_mRegions.Get(address).Value()->DeletePage(address);
    }

    else
    {
        PANIC("Tried to unmap page outside any region.");
    }
    
    m_lock.Unlock();
}

Memory::VM::Region::Region()
    : Userspace(0), AutoAllocate(0), CoreRegion(0), KernelRegion(0), CacheDisable(0), WriteThrough(0),
      ChangedSinceSwitch(0), FileMapped(0), Shared(0), MMIO(0), ReadOnly(0), KillOnRead(0), Base(0),
      End(0), Parent(nullptr), SharedAddressSpace(nullptr), MappedFile(nullptr)
{
}

Memory::VM::Region::~Region()
{
    for (auto it = m_mPages.Begin(), end = m_mPages.End(); it != end; it++)
    {
        delete it.Value();
    }
}

void Memory::VM::Region::AddPage(Memory::VM::Page * pPage)
{
    m_lock.Lock();

    if (m_mPages.Get(pPage->VirtualAddress))
    {
        PANIC("Tried to map already mapped page.");
    }

    if (!pPage->Allocated && pPage->PhysicalAddress)
    {
        Parent->m_pPML4->Map(pPage->VirtualAddress, 4096, pPage->PhysicalAddress);
    }

    m_mPages.Insert(pPage->VirtualAddress, pPage);
    
    m_lock.Unlock();
}

void Memory::VM::Region::DeletePage(Memory::VM::Page * pPage)
{
    m_lock.Lock();

    if (auto it = m_mPages.Get(pPage->VirtualAddress))
    {
        if (it.Value() != pPage)
        {
            PANIC("Tried to remove not matching page.");
        }

        if (!m_mPages.Remove(it))
        {
            PANIC("Failed to remove page.");
        }
    }

    else
    {
        PANIC("Tried to remove unexistent page.");
    }

    m_lock.Unlock();
}

void Memory::VM::Region::DeletePage(uint64 iAddress)
{
    m_lock.Lock();

    if (auto it = m_mPages.Get(iAddress))
    {
        m_mPages.Remove(it);
    }

    else
    {
        PANIC("Tried to remove unexistent page.");
    }

    m_lock.Unlock();
}

Memory::VM::Page::Page()
    : Allocated(0), Userspace(0), AutoAllocate(0), CoWCopy(0), CowBase(0), CacheDisable(0), WriteThrough(0),
      ChangedSinceSwitch(0), MMIOPage(0), ReadOnly(0), KillOnWrite(0), Global(0), PhysicalAddress(0),
      VirtualAddress(0), Parent(nullptr), CoWBase(nullptr), CoWCounter(0)
{
}

Memory::VM::Page::~Page()
{
    if (CoWCopy || CowBase)
    {
        if (CoWCounter == 1)
        {
            CoWCopies[0]->CoWBase = nullptr;
            CoWCopies[0]->CoWCounter = 0;
            CoWCopies[0]->CoWCopy = 0;

            return;
        }

        Page * base;
        
        if (CowBase)
        {
            base = CoWCopies[0];
        }

        for (auto it = CoWCopies.Begin(), end = CoWCopies.End(); it != end; ++it)
        {
            (*it)->CoWCounter--;

            if (CowBase)
            {
                (*it)->CoWBase = base;

                if (it == CoWCopies.Begin())
                {
                    (*it)->CowBase = true;
                }
            }
        }
    }
}

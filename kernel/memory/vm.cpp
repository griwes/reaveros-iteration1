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

}

void Memory::VM::AddressSpace::AddRegion(uint64 base, uint64 limit, Region * region)
{
    m_lock.Lock();
    
    if (region == nullptr)
    {
        Region * region = new Region;
        region->Base = base;
        region->End = base + limit;
    }

    auto it = Regions.Begin() + 1;
    for (; it != Regions.End(); it++)
    {
        if ((*it - 1)->Base == base)
        {
            PANIC();
        }

        if ((*it - 1)->Base < base && (*(it))->Base > base)
        {
            if ((*it - 1)->End < base && (*(it))->Base < base + limit)
            {
                break;
            }

            else
            {
                PANIC();
            }
        }
    }

    Regions.Insert(region, it);

    m_lock.Lock();
}

void Memory::VM::AddressSpace::AddRegion(Memory::VM::Region * pRegion)
{
    AddRegion(pRegion->Base, pRegion->End, pRegion);
}

void Memory::VM::AddressSpace::RemoveRegion(uint64 base)
{
    m_lock.Lock();
    
    auto it = Regions.Begin() + 1;
    for (; it != Regions.End(); it++)
    {
        if ((*it - 1)->Base == base)
        {
            Regions.Remove(it - 1);
            return;
        }

        if ((*it - 1)->Base < base && (*(it))->Base > base)
        {
            PANIC();
        }
    }

    m_lock.Unlock();
}

void Memory::VM::AddressSpace::RemoveRegion(Memory::VM::Region * pRegion)
{
    RemoveRegion(pRegion->Base);
}

void Memory::VM::AddressSpace::MapPage(uint64 address)
{
    MapPages(address, 4096, (Scheduler::Initialized ? Memory::CorePages->Pop() : Memory::GlobalPages->Pop()));
}

void Memory::VM::AddressSpace::MapPage(Memory::VM::Page * )
{

}

void Memory::VM::AddressSpace::UnmapPage(uint64 )
{

}

Memory::VM::Region::Region()
    : Userspace(0), AutoAllocate(0), CoreRegion(0), KernelRegion(0), CacheDisable(0), WriteThrough(0),
      ChangedSinceSwitch(0), FileMapped(0), Shared(0), MMIO(0), ReadOnly(0), KillOnRead(0), Base(0),
      End(0), Parent(nullptr), SharedAddressSpace(nullptr), MappedFile(nullptr)
{
}

Memory::VM::Region::~Region()
{

}

void Memory::VM::Region::AddPage(Memory::VM::Page * )
{

}

void Memory::VM::Region::DeletePage(Memory::VM::Page * )
{

}

Memory::VM::Page::Page()
{

}

Memory::VM::Page::~Page()
{

}
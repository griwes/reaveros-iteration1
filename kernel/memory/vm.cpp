#include "vm.h"

Memory::VM::AddressSpace::AddressSpace(uint64 base)
    : m_pPML4(new Paging::PML4(base))
{
}

Memory::VM::AddressSpace::~AddressSpace()
{

}

void Memory::VM::AddressSpace::AddRegion(uint64 base, uint64 limit, Region * region)
{
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
}

void Memory::VM::AddressSpace::AddRegion(Memory::VM::Region * pRegion)
{
    AddRegion(pRegion->Base, pRegion->End, pRegion);
}

void Memory::VM::AddressSpace::RemoveRegion(uint64 base)
{
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
}

void Memory::VM::AddressSpace::RemoveRegion(Memory::VM::Region * pRegion)
{
    RemoveRegion(pRegion->Base);
}

void Memory::VM::AddressSpace::MapPage(uint64 )
{

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

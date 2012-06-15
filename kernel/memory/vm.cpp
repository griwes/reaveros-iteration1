#include "vm.h"

Memory::VM::AddressSpace::AddressSpace()
{

}

Memory::VM::AddressSpace::~AddressSpace()
{

}

void Memory::VM::AddressSpace::AddRegion(Memory::VM::Region * )
{

}

void Memory::VM::AddressSpace::RemoveRegion(Memory::VM::Region * )
{

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

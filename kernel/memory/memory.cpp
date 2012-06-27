/**
 * ReaverOS
 * kernel/memory/memory.cpp
 * Main memory subsystem implementation file.
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
#include "../processor/processor.h"
#include "memorymap.h"
#include "heap.h"
#include "paging.h"
#include "../lib/stack.h"
#include "../lib/list.h"
#include "vm.h"
#include "vmm.h"

const uint64 TopMemory = 256ull * 1024 * 1024 * 1024 * 1024;

namespace Memory
{
    void * PlacementAddress = 0;
    Memory::MemoryMap * SystemMemoryMap = 0;
    Memory::Heap * KernelHeap = 0;
    uint64 StackStart = 0;
    Lib::Stack * GlobalPages = 0;
    Lib::Stack * CorePages = 0;
    Lib::Stack * PagingStructures = 0;
    Lib::Stack * VMPages = 0;
    Lib::Stack * VMRegions = 0;
    Lib::Stack * VMAddressSpaces = 0;

    uint64 PagingStructuresCount = 0;
    uint64 VMPagesCount = 0;
    uint64 VMRegionsCount = 0;
    uint64 VMAddressSpacesCount = 0;
    
    VM::Region * KernelRegion = 0;
    VM::Region * VideoBackbufferRegion = 0;
    VM::Region * VideoMemoryRegion = 0;
    VM::Region * KernelHeapRegion = 0;
    VM::Region * PagingStructuresPoolRegion = 0;
    VM::Region * PagingStructuresPoolStackRegion = 0;
    VM::Region * GlobalPageStackRegion = 0;
    VM::Region * VMPagePoolRegion = 0;
    VM::Region * VMPagePoolStackRegion = 0;
    VM::Region * VMRegionPoolRegion = 0;
    VM::Region * VMRegionPoolStackRegion = 0;
    VM::Region * VMAddressSpacePoolRegion = 0;
    VM::Region * VMAddressSpacePoolStackRegion = 0;
    VM::Region * CoreDataRegion = 0;
    VM::Region * CorePageStackRegion = 0;
    VM::AddressSpace * CurrentVAS = 0;
    uint64 AvailableMemory = 0;
}

void * operator new(uint64 iSize)
{
    if (!Memory::PlacementAddress)
    {
        return Memory::KernelHeap->Alloc(iSize);
    }

    void * p = Memory::PlacementAddress;
    uint64 _ = (uint64)Memory::PlacementAddress + iSize;
    Memory::PlacementAddress = (void *)_;

    return p;
}

// hope no-one will ever try to free placed memory...
void operator delete(void * pPointer)
{
    if (!Memory::PlacementAddress)
    {
        Memory::KernelHeap->Free(pPointer);
    }
}

void * operator new[](uint64 iSize)
{
    return operator new(iSize);
}

void operator delete[](void * pPointer)
{
    if (!Memory::PlacementAddress)
    {
        Memory::KernelHeap->Free(pPointer);
    }
}

void Memory::AlignPlacementToPage()
{
    Memory::PlacementAddress = Memory::AlignToNextPage((uint64)Memory::PlacementAddress);
}

void * Memory::AlignToNextPage(uint64 p)
{
    p += 4095;
    p &= ~(uint64)4095;

    return (void *)p;
}

void Memory::PreInitialize(void * pPlacementAddress)
{
    Memory::PlacementAddress = pPlacementAddress;
    
    return;
}

void Memory::Initialize(Memory::MemoryMapEntry * pMemMap, uint32 iMemoryMapSize)
{
    Memory::SystemMemoryMap = new MemoryMap(pMemMap, iMemoryMapSize);

    Memory::AvailableMemory = SystemMemoryMap->CountUsableMemory();

    Memory::RemapKernel();
    Memory::GlobalPages = new Lib::Stack(Memory::SystemMemoryMap, Memory::VM::GlobalPageStackBase);
    Memory::KernelHeap = new Heap(Memory::VM::KernelHeapBase, Memory::VM::KernelHeapLimit);
        
    return;
}

void Memory::InitializeRegions()
{
    KernelRegion = new VM::Region;
    KernelRegion->Base = VM::KernelBase;
    KernelRegion->End = VM::KernelBase + VM::KernelLimit;
    KernelRegion->KernelRegion = true;

    VideoBackbufferRegion = new VM::Region;
    VideoBackbufferRegion->Base = VM::VideoBackbufferBase;
    VideoBackbufferRegion->End = VM::VideoBackbufferBase + VM::VideoBackbufferLimit;
    VideoBackbufferRegion->KernelRegion = true;

    VideoMemoryRegion = new VM::Region;
    VideoMemoryRegion->Base = VM::VideoMemoryBase;
    VideoMemoryRegion->End = VM::VideoMemoryBase + VM::VideoMemoryLimit;
    VideoMemoryRegion->KernelRegion = true;

    KernelHeapRegion = new VM::Region;
    KernelHeapRegion->Base = VM::KernelHeapBase;
    KernelHeapRegion->End = VM::KernelHeapBase + VM::KernelHeapLimit;
    KernelHeapRegion->KernelRegion = true;

    PagingStructuresPoolRegion = new VM::Region;
    PagingStructuresPoolRegion->Base = VM::PagingStructuresPoolBase;
    PagingStructuresPoolRegion->End = VM::PagingStructuresPoolBase + VM::PagingStructuresPoolLimit;
    PagingStructuresPoolRegion->KernelRegion = true;

    PagingStructuresPoolStackRegion = new VM::Region;
    PagingStructuresPoolStackRegion->Base = VM::PagingStructuresPoolStackBase;
    PagingStructuresPoolStackRegion->End = VM::PagingStructuresPoolStackBase + VM::PagingStructuresPoolStackLimit;
    PagingStructuresPoolStackRegion->KernelRegion = true;

    GlobalPageStackRegion = new VM::Region;
    GlobalPageStackRegion->Base = VM::GlobalPageStackBase;
    GlobalPageStackRegion->End = VM::GlobalPageStackBase + VM::GlobalPageStackLimit;
    GlobalPageStackRegion->KernelRegion = true;

    VMPagePoolRegion = new VM::Region;
    VMPagePoolRegion->Base = VM::VMPagePoolBase;
    VMPagePoolRegion->End = VM::VMPagePoolBase + VM::VMPagePoolLimit;
    VMPagePoolRegion->KernelRegion = true;

    VMPagePoolStackRegion = new VM::Region;
    VMPagePoolStackRegion->Base = VM::VMPagePoolStackBase;
    VMPagePoolStackRegion->End = VM::VMPagePoolStackBase + VM::VMPagePoolStackLimit;
    VMPagePoolStackRegion->KernelRegion = true;

    VMRegionPoolRegion = new VM::Region;
    VMRegionPoolRegion->Base = VM::VMRegionPoolBase;
    VMRegionPoolRegion->End = VM::VMRegionPoolBase + VM::VMRegionPoolLimit;
    VMRegionPoolRegion->KernelRegion = true;

    VMRegionPoolStackRegion = new VM::Region;
    VMRegionPoolStackRegion->Base = VM::VMRegionPoolStackBase;
    VMRegionPoolStackRegion->End = VM::VMRegionPoolStackBase + VM::VMRegionPoolStackLimit;
    VMRegionPoolStackRegion->KernelRegion = true;

    VMAddressSpacePoolRegion = new VM::Region;
    VMAddressSpacePoolRegion->Base = VM::VMAddressSpacePoolBase;
    VMAddressSpacePoolRegion->End = VM::VMAddressSpacePoolBase + VM::VMAddressSpacePoolLimit;
    VMAddressSpacePoolRegion->KernelRegion = true;

    VMAddressSpacePoolStackRegion = new VM::Region;
    VMAddressSpacePoolStackRegion->Base = VM::VMAddressSpacePoolStackBase;
    VMAddressSpacePoolStackRegion->End = VM::VMAddressSpacePoolStackBase + VM::VMAddressSpacePoolStackLimit;
    VMAddressSpacePoolStackRegion->KernelRegion = true;

    CurrentVAS->AddRegion(KernelRegion);
    CurrentVAS->AddRegion(VideoBackbufferRegion);
    CurrentVAS->AddRegion(VideoMemoryRegion);
    CurrentVAS->AddRegion(KernelHeapRegion);
    CurrentVAS->AddRegion(PagingStructuresPoolRegion);
    CurrentVAS->AddRegion(PagingStructuresPoolStackRegion);
    CurrentVAS->AddRegion(GlobalPageStackRegion);
    CurrentVAS->AddRegion(VMPagePoolRegion);
    CurrentVAS->AddRegion(VMPagePoolStackRegion);
    CurrentVAS->AddRegion(VMRegionPoolRegion);
    CurrentVAS->AddRegion(VMRegionPoolStackRegion);
    CurrentVAS->AddRegion(VMAddressSpacePoolRegion);
    CurrentVAS->AddRegion(VMAddressSpacePoolStackRegion);

    PagingStructures = new Lib::Stack(0, 1024 * 1024, VM::PagingStructuresPoolStackBase);
    VMPages = new Lib::Stack(0, 1024 * 1024, VM::VMPagePoolStackBase);
    VMRegions = new Lib::Stack(0, 1024, VM::VMRegionPoolStackBase);
    VMAddressSpaces = new Lib::Stack(0, 1024, VM::VMAddressSpacePoolStackBase);

    PagingStructuresCount = 1024 * 1024;
    VMPagesCount = 1024 * 1024;
    VMRegionsCount = 1024;
    VMAddressSpacesCount = 1024;
}

void Memory::RemapKernel()
{
    Memory::MemoryMapEntry * p = (Memory::MemoryMapEntry *)0xFFFFFFFFFFFFFFFF;
    
    for (uint64 i = 0; i < Memory::SystemMemoryMap->GetNumberOfEntries(); i++)
    {
        if (Memory::SystemMemoryMap->GetEntries()[i].Type() == 0xffff)
        {
            p = &Memory::SystemMemoryMap->GetEntries()[i];
            break;
        }
    }

    // unfortunately, there is no way to panic yet...
    if (p == (Memory::MemoryMapEntry *)0xFFFFFFFFFFFFFFFF)
    {
        for (;;);
    }
    
    CurrentVAS = new VM::AddressSpace(p->Base());
    
    VMM::MapPages(0xFFFFFFFF80000000, p->Length() - 20 * 1024, p->Base());
    VMM::MapPages(0xFFFFFFFF80000000 + p->Length() - 16 * 1024, 16 * 1024, p->End() - 16 * 1024);
    
    Memory::StackStart = 0xFFFFFFFF80000000 + p->Length();

    CurrentVAS->SetActive();
    Paging::PML4::s_iBase = 0;

    return;
}

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

namespace Memory
{
    void * pPlacementAddress = 0;
    Memory::MemoryMap * pMemoryMap = 0;
    Memory::Heap * KernelHeap = 0;
    Paging::PML4 * KernelPML4 = 0;
    Paging::PageDirectory * KernelSpace[2] = {0, 0};
    uint64 StackStart = 0;
}

void * operator new(uint64 iSize)
{
    if (!Memory::pPlacementAddress)
    {
        return Memory::KernelHeap->Alloc(iSize);
    }

    void * p = Memory::pPlacementAddress;
    Memory::pPlacementAddress += iSize;

    return p;
}

// hope no-one will ever try to free placed memory...
void operator delete(void * pPointer)
{
    Memory::KernelHeap->Free(pPointer);
}

void * operator new[](uint64 iSize)
{
    return operator new(iSize);
}

void operator delete[](void * pPointer)
{
    Memory::KernelHeap->Free(pPointer);
}

void Memory::AlignPlacementToPage()
{
    Memory::pPlacementAddress = Memory::AlignToNextPage((uint64)Memory::pPlacementAddress);
}

void * Memory::AlignToNextPage(uint64 p)
{
    p += 4095;
    p &= ~4095;
    
    return (void *)p;
}

void Memory::PreInitialize(void * pPlacementAddress)
{
    Memory::pPlacementAddress = pPlacementAddress;
    
    return;
}

void Memory::Initialize(Memory::MemoryMapEntry * pMemMap, uint32 iMemoryMapSize)
{
    Memory::pMemoryMap = new MemoryMap(pMemMap, iMemoryMapSize);
    
    Memory::RemapKernel();
    dbg;
    Memory::CreateFreePageStack();
    Memory::KernelHeap = new Heap;

    return;
}

void Memory::RemapKernel()
{
    Memory::MemoryMapEntry * p = (Memory::MemoryMapEntry *)0xFFFFFFFFFFFFFFFF;
    
    for (uint64 i = 0; i < Memory::pMemoryMap->GetNumberOfEntries(); i++)
    {
        if (Memory::pMemoryMap->GetEntries()[i].Type() == 0xffff)
        {
            p = &Memory::pMemoryMap->GetEntries()[i];
            break;
        }
    }

    // unfortunately, there is no way to panic yet...
    if (p == (Memory::MemoryMapEntry *)0xFFFFFFFFFFFFFFFF)
    {
        for (;;);
    }

    Memory::AlignPlacementToPage();

    Memory::KernelPML4 = new Paging::PML4;
    Memory::KernelPML4->Map(0xFFFFFFFF80000000, p->Length() - 20 * 1024, p->Base());        
    Memory::KernelPML4->Map(0xFFFFFFFF80000000 + p->Length() - 16 * 1024, 16 * 1024, p->End() - 16 * 1024);
    // 1-page gap above is current kernel stack control - 4 KiB should be enough for boot-up kernel stack

    Memory::StackStart = 0xFFFFFFFF80000000 + p->Length();
    Processor::LoadCR3((uint64)Memory::KernelPML4 - 0xFFFFFFFF80000000 + p->Base());
    
    return;
}

void Memory::CreateFreePageStack()
{

}

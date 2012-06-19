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

namespace Memory
{
    void * PlacementAddress = 0;
    Memory::MemoryMap * SystemMemoryMap = 0;
    Memory::Heap * KernelHeap = 0;
    uint64 StackStart = 0;
    Lib::Stack * GlobalPages = 0;
    Lib::Stack * CorePages = 0;
    VM::Region * KernelRegion = 0;
    VM::AddressSpace * CurrentVAS = 0;
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

    Memory::RemapKernel();
    Memory::GlobalPages = new Lib::Stack(Memory::SystemMemoryMap, Memory::VM::FreePageStackBase);
    Memory::KernelHeap = new Heap(Memory::VM::HeapBase, Memory::VM::HeapLimit);

    Memory::PlacementAddress = (void *)0;

    return;
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
    PANIC();
    CurrentVAS->m_pPML4->m_iBase = 0;

    return;
}

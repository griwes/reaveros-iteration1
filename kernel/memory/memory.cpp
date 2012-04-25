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
 * including commercial applications, adn to alter it and redistribute it
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

// hope no-one will ever try to free placement address...
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

namespace Memory
{
    void * pPlacementAddress = 0;
}

void Memory::PreInitialize(void * pPlacementAddress)
{
    Memory::pPlacementAddress = pPlacementAddress;
    
    return;
}

void Memory::Initialize(Memory::MemoryMapEntry * pMemMap, uint32 iMemoryMapSize)
{
    MemoryMap * pMemoryMap = new pMemoryMap(pMemMap, iMemoryMapSize);
}

template<typename T>
void Memory::Zero(T * p)
{
    uint32 iSize = sizeof(T);

    for (uint32 i = 0; i < iSize; i++)
    {
        *(char *)(p++) = 0;
    }
}

template<typename T>
void Memory::Zero(T * p, uint32 iCount)
{
    uint32 iSize = sizeof(T) * iCount;

    for (uint32 i = 0; i < iSize; i++)
    {
        *(char *)(p++) = 0;
    }
}
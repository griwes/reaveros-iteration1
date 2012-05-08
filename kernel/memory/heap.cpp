/**
 * ReaverOS
 * kernel/memory/heap.cpp
 * Kernel heap implementation.
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

#include "heap.h"
#include "vmm.h"

#include <vector>
#include <alloca.h>

Memory::Heap::Heap()
{
    
}

// oh noes, big and evil constructor initializer list...
Memory::Heap::Heap(uint64 start)
        : m_pBiggest((AllocationBlockHeader *)start), m_pSmallest((AllocationBlockHeader *)start),
          m_pBiggest1024((AllocationBlockHeader *)start), m_pSmallest1024((AllocationBlockHeader *)start),
          m_pBiggest256((AllocationBlockHeader *)start), m_pSmallest256((AllocationBlockHeader *)start),
          m_pBiggest64((AllocationBlockHeader *)start), m_pSmallest64((AllocationBlockHeader *)start),
          m_pBiggest16((AllocationBlockHeader *)start), m_pSmallest16((AllocationBlockHeader *)start),
          m_iStart(start), m_iEnd(start + 4 * 4 * 1024)
{
    Memory::VMM::MapPage(start);
    Memory::VMM::MapPage(start + 4096);
    Memory::VMM::MapPage(start + 2 * 4096);
    Memory::VMM::MapPage(start + 3 * 4096);

    this->m_pBiggest->Magic = 0xFEA7EFA1;
    this->m_pBiggest->Size = 4 * 4 * 1024 - sizeof(AllocationBlockHeader) - sizeof(AllocationBlockFooter);
    this->m_pBiggest->Footer()->Magic = 0xFEA7EFA1;
    this->m_pBiggest->Footer()->Header = this->m_pBiggest;
}

Memory::Heap::~Heap()
{
    for (uint64 s = this->m_iStart; s < this->m_iEnd; s += 4096)
    {
        Memory::VMM::UnmapPage(s);
    }
}

void * Memory::Heap::Alloc(uint64 iSize)
{
    this->m_pLock.Lock();

    AllocationBlockHeader * list = this->_select_list(iSize);

    this->m_pLock.Unlock();
}

void Memory::Heap::Free(void * pAddr)
{
    this->m_pLock.Lock();

    AllocationBlockHeader * allocation = (AllocationBlockHeader *)((char *)pAddr - sizeof(AllocationBlockHeader));
    AllocationBlockHeader * list = this->_select_list(allocation->Size);

    this->m_pLock.Unlock();
}

Memory::AllocationBlockHeader * Memory::Heap::_select_list(uint64 iSize)
{
    if (iSize <= 4)
    {
        return this->m_pSmallest;
    }
    
    else if (iSize <= 16)
    {
        if (iSize <= 10)
        {
            return this->m_pSmallest16;
        }
        
        else
        {
            return this->m_pBiggest16;
        }
    }
    
    else if (iSize <= 64)
    {
        if (iSize <= 40)
        {
            return this->m_pSmallest64;
        }
        
        else
        {
            return this->m_pBiggest64;
        }
    }
    
    else if (iSize <= 256)
    {
        if (iSize <= 160)
        {
            return this->m_pSmallest256;
        }
        
        else
        {
            return this->m_pBiggest256;
        }
    }
    
    else if (iSize <= 1024)
    {
        if (iSize <= 640)
        {
            return this->m_pSmallest1024;
        }
        
        else
        {
            return this->m_pBiggest1024;
        }
    }
    
    else
    {
        return this->m_pBiggest;
    }
}

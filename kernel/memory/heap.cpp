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

Memory::Heap::Heap(uint64 start, uint64 limit)
        : m_pBiggest((AllocationBlockHeader *)start), m_pSmallest((AllocationBlockHeader *)start),
          m_iStart(start), m_iEnd(start + 64 * 1024), m_iLimit(limit)
{
    Memory::VMM::MapPages(this->m_iStart, this->m_iEnd);

    this->m_pBiggest->Magic = 0xFEA7EFA1;
    this->m_pBiggest->Size = 64 * 1024 - sizeof(AllocationBlockHeader) - sizeof(AllocationBlockFooter);
    this->m_pBiggest->Bigger = nullptr;
    this->m_pBiggest->Smaller = nullptr;
    this->m_pBiggest->Flags = 0;
    
    this->m_pBiggest->Footer()->Magic = 0xFEA7EFA1;
    this->m_pBiggest->Footer()->Header = this->m_pBiggest;
}

Memory::Heap::~Heap()
{
    VMM::UnmapPages(m_iStart, m_iEnd - m_iStart);
}

void * Memory::Heap::Alloc(uint64 iSize)
{
    this->m_lock.Lock();

    this->_check_sanity();
    
    iSize += 15;
    iSize &= ~(uint64)15;
    
    if (iSize == 0)
    {
        this->m_lock.Unlock();
        return nullptr;
    }
    
    AllocationBlockHeader * list = this->_find_closest(iSize);
    while (list == nullptr || list->Size < iSize)
    {
        this->_expand();
        this->_check_sanity();
        
        list = this->_find_closest(iSize);
    }
    
    void * ret = this->_allocate(list, iSize);
    this->m_lock.Unlock();
    return ret;
}

void * Memory::Heap::AllocAligned(uint64 iSize)
{
    this->m_lock.Lock();
    
    this->_check_sanity();
    
    if (iSize == 0)
    {
        this->m_lock.Unlock();
        return nullptr;
    }
    
    AllocationBlockHeader * last = ((AllocationBlockFooter *)(this->m_iEnd - sizeof(AllocationBlockFooter)))->Header;
    
    if (!this->_is_free(last))
    {
        this->_expand();
    }
    
    uint64 iModSize = iSize % 4096;
    
    if (iModSize > 0)
    {
        this->_expand();
    }
    
    if (iModSize + sizeof(AllocationBlockFooter) > 4096)
    {
        this->_expand();
    }
    
    for (uint64 i = 0; i < iSize - iModSize; i += 4096)
    {
        this->_expand();
    }
    
    AllocationBlockHeader * block = (this->_is_free(last) ? last : last->Next());
    uint64 pAddress = (uint64)block + sizeof(AllocationBlockHeader);
    uint64 pAddressAligned = pAddress + 4095;
    pAddressAligned &= ~(uint64)4095;
    
    uint64 iBlockSize = iSize + (pAddressAligned - pAddress);
    
    // FIXME: this way of allocating wastes up to 4095 bytes
    // FIXME: add _break() (counter-_merge()) to solve this problem
    this->_allocate(block, iBlockSize);
    
    this->m_lock.Unlock();

    return (void *)pAddressAligned;
}

void Memory::Heap::Free(void * pAddress)
{
    this->m_lock.Lock();
    
    this->_check_sanity();
    
    if (pAddress == nullptr)
    {
        return;
    }
    
    this->_validate(pAddress);
    
    AllocationBlockHeader * newhead = (AllocationBlockHeader *)((uint8 *)pAddress - sizeof(AllocationBlockHeader));
    newhead->Flags ^= 1;
    this->_insert(newhead);

    this->m_lock.Unlock();
}

void Memory::Heap::_check_sanity()
{
    if (this->m_pBiggest == nullptr || this->m_pSmallest == nullptr)
    {
        if (this->m_pBiggest != nullptr || this->m_pSmallest != nullptr)
        {
            PANIC("Only one nullptr.");
        }
    }
    
    if (m_iEnd > m_iStart + m_iLimit)
    {
        PANIC("Heap limit exceeded.");
    }
}

void Memory::Heap::_expand()
{
    this->m_iEnd += 4096;
    this->_check_sanity();

    Memory::VMM::MapPage(this->m_iEnd - 4096);
    
    AllocationBlockHeader * newhead = (AllocationBlockHeader *)(this->m_iEnd - 4096);
    newhead->Magic = 0xFEA7EFA1;
    newhead->Size = 4096 - sizeof(AllocationBlockHeader) - sizeof(AllocationBlockFooter);
    newhead->Flags = 1 << 1;
    newhead->Footer()->Magic = 0xFEA7EFA1;
    newhead->Footer()->Header = newhead;
    
    newhead->Smaller = nullptr;
    newhead->Bigger = nullptr;
    
    this->_insert(newhead);
}

void Memory::Heap::_insert(AllocationBlockHeader * newhead)
{
    this->_check_sanity();
    
    if (this->m_pBiggest == nullptr)
    {
        this->m_pBiggest = newhead;
        this->m_pSmallest = newhead;
        newhead->Bigger = nullptr;
        newhead->Smaller = nullptr;
        
        return;
    }
    
    if (this->_is_free(newhead->Previous()))
    {
        this->_merge(newhead->Previous(), newhead);
        return;
    }
    
    if (this->_is_free(newhead->Next()))
    {
        this->_merge(newhead, newhead->Next());
        return;
    }
    
    if ((newhead->Flags & 2) != 2 && newhead->Size + sizeof(AllocationBlockHeader)
        + sizeof(AllocationBlockFooter) >= 4096 && this->m_iEnd == (uint64)newhead->Footer()
        + sizeof(AllocationBlockFooter))
    {
        this->_shrink(newhead);
        return;
    }
    
    AllocationBlockHeader * list = this->_find_closest(newhead->Size);
    
    if (newhead->Size <= list->Size)
    {
        newhead->Smaller = list->Smaller;
        newhead->Bigger = list;
        list->Smaller = newhead;
        
        if (this->m_pSmallest == list)
        {
            this->m_pSmallest = newhead;
        }
    }
    
    else
    {
        newhead->Bigger = list->Bigger;
        newhead->Smaller = list;
        list->Bigger = newhead;
        
        if (this->m_pBiggest == list)
        {
            this->m_pBiggest = newhead;
        }
    }
    
    newhead->Flags ^= 2;
    
    this->_check_sanity();
}

void Memory::Heap::_merge(Memory::AllocationBlockHeader * first, Memory::AllocationBlockHeader * second)
{
    if (first == second)
    {
        PANIC("Tried to merge AllocationBlockHeader with itself.");
    }
    
    if ((uint64)first > (uint64)second)
    {
        return this->_merge(second, first);
    }
    
    if (first->Next() != second)
    {
        PANIC("Tried to merge not adjacent AllocationBlockHeader.");
    }
    
    if (first->Bigger != nullptr)
    {
        first->Bigger->Smaller = first->Smaller;
    }
    
    if (first->Smaller != nullptr)
    {
        first->Smaller->Bigger = first->Bigger;
    }
    
    if (second->Bigger != nullptr)
    {
        second->Bigger->Smaller = second->Smaller;
    }
    
    if (second->Smaller != nullptr)
    {
        second->Smaller->Bigger = second->Bigger;
    }
    
    if (first == this->m_pBiggest)
    {
        this->m_pBiggest = first->Smaller;
    }
    
    if (first == this->m_pSmallest)
    {
        this->m_pSmallest = first->Bigger;
    }
    
    if (second == this->m_pBiggest)
    {
        this->m_pBiggest = second->Smaller;
    }
    
    if (second == this->m_pSmallest)
    {
        this->m_pSmallest = second->Bigger;
    }
    
    first->Size += second->Size + sizeof(AllocationBlockHeader) + sizeof(AllocationBlockFooter);
    first->Footer()->Header = first;
    
    if ((second->Flags & 2) == 2)
    {
        first->Flags |= 2;
    }
    
    this->_insert(first);
}

void * Memory::Heap::_allocate(Memory::AllocationBlockHeader * block, uint64 iSize)
{
    if (block->Size == iSize || block->Size < iSize + sizeof(AllocationBlockHeader) + sizeof(AllocationBlockFooter) + 1)
    {
        block->Flags |= 1;
        
        if (block->Bigger != nullptr)
        {
            block->Bigger->Smaller = block->Smaller;
        }
        
        if (block->Smaller != nullptr)
        {
            block->Smaller->Bigger = block->Bigger;
        }
        
        if (block == this->m_pBiggest)
        {
            this->m_pBiggest = block->Smaller;
        }
        
        if (block == this->m_pSmallest)
        {
            this->m_pSmallest = block->Bigger;
        }
        
        return (void *)((uint8 *)block + sizeof(AllocationBlockHeader));
    }
    
    else
    {
        block->Flags |= 1;
        
        uint64 iOrigSize = block->Size;
        block->Size = iSize;
        block->Footer()->Magic = 0xFEA7EFA1;
        block->Footer()->Header = block;
        
        if (block->Bigger != nullptr)
        {
            block->Bigger->Smaller = block->Smaller;
        }
        
        if (block->Smaller != nullptr)
        {
            block->Smaller->Bigger = block->Bigger;
        }
        
        if (block == this->m_pBiggest)
        {
            this->m_pBiggest = block->Smaller;
        }
        
        if (block == this->m_pSmallest)
        {
            this->m_pSmallest = block->Bigger;
        }
        
        AllocationBlockHeader * newhead = block->Next();
        newhead->Magic = 0xFEA7EFA1;
        newhead->Flags = 0;
        newhead->Size = iOrigSize - iSize - sizeof(AllocationBlockHeader) - sizeof(AllocationBlockFooter);
        
        newhead->Footer()->Header = newhead;
        
        this->_insert(newhead);
        
        return (void *)((uint8 *)block + sizeof(AllocationBlockHeader));
    }
}

Memory::AllocationBlockHeader * Memory::Heap::_find_closest(uint64 iSize)
{
    AllocationBlockHeader * list = nullptr;
    
    if (this->m_pBiggest == nullptr)
    {
        return nullptr;
    }
    
    if (abs((int64)this->m_pBiggest->Size - (int64)iSize) < abs((int64)this->m_pSmallest->Size - (int64)iSize))
    {
        list = this->m_pBiggest;
        
        while (list->Size != iSize && list->Smaller != nullptr && list->Smaller->Size >= iSize)
        {
            list = list->Smaller;
        }
    }
    
    else
    {
        list = this->m_pSmallest;
        
        while (list->Size != iSize && list->Bigger != nullptr && list->Bigger->Size <= iSize)
        {
            list = list->Bigger;
        }
    }
    
    return list;
}

bool Memory::Heap::_is_free(Memory::AllocationBlockHeader * block)
{
    if ((uint64)block >= this->m_iStart && (uint64)block->Footer() + sizeof(AllocationBlockFooter) <= this->m_iEnd &&
        block->Magic == 0xFEA7EFA1 && block->Footer()->Magic == 0xFEA7EFA1 && (block->Flags & 1) == 0)
    {
        return true;
    }
    
    else
    {
        return false;
    }
}

void Memory::Heap::_shrink(Memory::AllocationBlockHeader * head)
{
    // TODO
    head->Flags |= 2;
    this->_insert(head);
}

void * Memory::Heap::_validate(void * pAddress, bool bShouldBeAllocated)
{
    AllocationBlockHeader * head = (AllocationBlockHeader *)((uint8 *)pAddress - sizeof(AllocationBlockHeader));
    if (head->Magic != 0xFEA7EFA1 || head->Footer()->Magic != 0xFEA7EFA1)
    {
        PANIC("Invalid magic number in allocation structures.");
    }
    
    if (bShouldBeAllocated && (head->Flags & 1) != 1)
    {
        PANIC("Not allocated block should be allocated.");
    }
    
    if (!bShouldBeAllocated && (head->Flags & 1) == 1)
    {
        PANIC("Allocated block shouldn't be allocated.");
    }
    
    return pAddress;
}

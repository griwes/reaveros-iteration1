/**
 * ReaverOS
 * kernel/memory/heap.h
 * Heap header file.
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

#ifndef __rose_kernel_memory_heap_h__
#define __rose_kernel_memory_heap_h__

#include "../types.h"
#include "../processor/synchronization.h"

namespace Memory
{
    struct AllocationBlockFooter;
    
    struct AllocationBlockHeader
    {
        uint32 Magic;
        uint32 Size;
        AllocationBlockHeader * Smaller;
        AllocationBlockHeader * Bigger;

        inline AllocationBlockFooter * Footer()
        {
            return (AllocationBlockFooter *)((uint8 *)this + sizeof(AllocationBlockHeader) + this->Size);
        }
    };

    struct AllocationBlockFooter
    {
        uint32 Magic;
        AllocationBlockHeader * Header;
    };

    class Heap
    {
    public:
        Heap();
        Heap(uint64);
        ~Heap();

        void * Alloc(uint64);
        void Free(void *);

    private:
        AllocationBlockHeader * m_pBiggest1024;
        AllocationBlockHeader * m_pSmallest1024;
        AllocationBlockHeader * m_pBiggest256;
        AllocationBlockHeader * m_pSmallest256;
        AllocationBlockHeader * m_pBiggest64;
        AllocationBlockHeader * m_pSmallest64;
        AllocationBlockHeader * m_pBiggest16;
        AllocationBlockHeader * m_pSmallest16;

        AllocationBlockHeader * m_pBiggest;
        AllocationBlockHeader * m_pSmallest;

        uint64 m_iStart, m_iEnd;

        Processor::Spinlock m_pLock;

        AllocationBlockHeader * _select_list(uint64);
    };
}

#endif
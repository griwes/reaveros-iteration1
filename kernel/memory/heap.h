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
    struct AllocationBlockHeader;
    
    struct AllocationBlockFooter
    {
        uint32 Magic;
        AllocationBlockHeader * Header;
    } __attribute__((__packed__));
    
    struct AllocationBlockHeader
    {
        uint32 Magic;
        uint32 Flags;
        uint64 Size;
        AllocationBlockHeader * Smaller;
        AllocationBlockHeader * Bigger;

        inline AllocationBlockFooter * Footer()
        {
            return (AllocationBlockFooter *)((uint8 *)this + sizeof(AllocationBlockHeader) + this->Size);
        }

        inline AllocationBlockHeader * Previous()
        {
            return ((AllocationBlockFooter *)((uint8 *)this - sizeof(AllocationBlockFooter)))->Header;
        }

        inline AllocationBlockHeader * Next()
        {
            return (AllocationBlockHeader *)((uint8 *)this->Footer() + sizeof(AllocationBlockFooter));
        }
    } __attribute__((__packed__));

    class Heap
    {
    public:
        Heap(uint64);
        ~Heap();

        void * Alloc(uint64);
        void * AllocAligned(uint64);    // for allocating paging structures
        void Free(void *);

    private:
        AllocationBlockHeader * m_pBiggest;
        AllocationBlockHeader * m_pSmallest;

        uint64 m_iStart, m_iEnd, m_iLimit;
        
        Processor::Spinlock m_lock;

        void _check_sanity();
        void _expand();
        void _insert(AllocationBlockHeader *);
        void * _validate(void *, bool = true);
        void * _allocate(AllocationBlockHeader *, uint64);
        AllocationBlockHeader * _find_closest(uint64);
        bool _is_free(AllocationBlockHeader *);
        void _merge(AllocationBlockHeader *, AllocationBlockHeader *);
        void _shrink(AllocationBlockHeader *);
    };
}

#endif
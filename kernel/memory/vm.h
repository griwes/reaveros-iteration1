/**
 * ReaverOS
 * kernel/memory/vm.h
 * Virtual memory declaration.
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

#ifndef __rose_kernel_memory_vm_h__
#define __rose_kernel_memory_vm_h__

#include "../types.h"
#include "paging.h"
#include "../processor/synchronization.h"
#include "../processor/processor.h"
#include "../lib/vector.h"
#include "../lib/rangemap.h"
#include "memory.h"
#include "../lib/stack.h"
#include "heap.h"

namespace Scheduler
{
    class Process;
}

class File;

namespace Memory
{    
    namespace VMM
    {
        void * AllocPagingPages();
    }

    namespace VM
    {
        class AddressSpace;
        class Region;
        
        class Page
        {
        public:
            Page();
            ~Page();

            uint64 Allocated:1;
            uint64 Userspace:1;
            uint64 AutoAllocate:1;
            uint64 CoWCopy:1;
            uint64 CowBase:1;
            uint64 CacheDisable:1;
            uint64 WriteThrough:1;
            uint64 ChangedSinceSwitch:1;
            uint64 MMIOPage:1;
            uint64 ReadOnly:1;
            uint64 KillOnWrite:1;
            uint64 Global:1;
            
            uint64 PhysicalAddress;
            uint64 VirtualAddress;

            Region * Parent;

            Page * CoWBase;
            Lib::Vector<Page *> CoWCopies;
            uint64 CoWCounter;

            Processor::Spinlock Lock;
        };

        class Region
        {
        public:
            Region();
            ~Region();

            void AddPage(Page *);
            void DeletePage(Page *);
            void DeletePage(uint64);

            uint64 Userspace:1;
            uint64 AutoAllocate:1;
            uint64 CoreRegion:1;
            uint64 KernelRegion:1;
            uint64 CacheDisable:1;
            uint64 WriteThrough:1;
            uint64 ChangedSinceSwitch:1;
            uint64 FileMapped:1;
            uint64 Shared:1;
            uint64 MMIO:1;
            uint64 ReadOnly:1;
            uint64 KillOnRead:1;

            uint64 Base;
            uint64 End;

            AddressSpace * Parent;
            AddressSpace * SharedAddressSpace;

            File * MappedFile;

        private:
            Processor::Lock * m_pLock;
            Lib::Trees::BinarySearch<uint64, Page *> m_mPages;
        };

        class AddressSpace
        {
        public:
            friend void Memory::RemapKernel();
            friend void * Memory::VMM::AllocPagingPages();
            friend void Lib::Stack::RegisterPages();
            friend void Memory::Heap::RegisterPages();

            friend class Region;
            friend class Page;
            
            AddressSpace(uint64 = 0);
            ~AddressSpace();

            void AddRegion(uint64, uint64, Region * = nullptr);
            void AddRegion(Region *);
            void RemoveRegion(uint64);
            void RemoveRegion(Region *);
            
            Region * GetRegion(uint64 addr)
            {
                return m_mRegions.Get(addr).Value();
            }
            
            void MapPage(uint64);
            void MapPage(uint64 s, uint64 b)
            {
                m_lock.Lock();
                m_pPML4->Map(s, 4096, b);
                m_lock.Unlock();
            }
            
            void MapPages(uint64 start, uint64 end)
            {
                while (start < end)
                {
                    MapPage(start);
                    start += 4096;
                }
            }

            void MapPages(uint64 s, uint64 l, uint64 b, bool cache = false)
            {
                m_lock.Lock();
                m_pPML4->Map(s, l, b, cache);
                m_lock.Unlock();
            }

            void MapPage(Page *);

            void UnmapPage(uint64);
            void UnmapPages(uint64 start, uint64 end)
            {
                while (start < end)
                {
                    UnmapPage(start);
                    start += 4096;
                }
            }

            void SetActive()
            {
                Processor::LoadCR3(m_pPML4->GetPhysicalAddress((uint64)m_pPML4));
            }

            Scheduler::Process * Parent;

        private:
            Processor::Spinlock m_lock;
            Paging::PML4 * m_pPML4;
            Lib::RangeMap<Region *> m_mRegions;
        };
        
        // kernel global addresses and limits
        const uint64 KernelBase = 0xFFFFFFFF80000000;
        const uint64 KernelLimit = 2ull * 1024 * 1024 * 1024;
        const uint64 VideoBackbufferLimit = 1024ull * 1024 * 1024;
        const uint64 VideoBackbufferBase = KernelBase - VideoBackbufferLimit;
        const uint64 VideoMemoryLimit = 1024ull * 1024 * 1024;
        const uint64 VideoMemoryBase = VideoBackbufferBase - VideoMemoryLimit;
        const uint64 KernelHeapLimit = 1024ull * 1024 * 1024 * 1024;
        const uint64 KernelHeapBase = VideoMemoryBase - KernelHeapLimit;
        const uint64 PagingStructuresPoolLimit = 1025ull * 1024 * 1024 * 1024;
        const uint64 PagingStructuresPoolBase = KernelHeapBase - PagingStructuresPoolLimit;
        const uint64 PagingStructuresPoolStackLimit = 1025ull * 1024 * 256;
        const uint64 PagingStructuresPoolStackBase = PagingStructuresPoolBase - PagingStructuresPoolStackLimit;
        const uint64 GlobalPageStackLimit = 512ull * 1024 * 1024 * 1024;
        const uint64 GlobalPageStackBase = PagingStructuresPoolStackBase - GlobalPageStackLimit;
        const uint64 VMPagePoolLimit = sizeof(Page) * 256ull * 1024 * 1024;
        const uint64 VMPagePoolBase = GlobalPageStackBase - VMPagePoolLimit;
        const uint64 VMPagePoolStackLimit = sizeof(Page *) * 256ull * 1024 * 1024;
        const uint64 VMPagePoolStackBase = VMPagePoolBase - VMPagePoolStackLimit;
        const uint64 VMRegionPoolLimit = sizeof(Region) * 1024ull * 1024;
        const uint64 VMRegionPoolBase = VMPagePoolStackBase - VMRegionPoolLimit;
        const uint64 VMRegionPoolStackLimit = sizeof(Region *) * 1024ull * 1024;
        const uint64 VMRegionPoolStackBase = VMRegionPoolBase - VMRegionPoolStackLimit;
        const uint64 VMAddressSpacePoolLimit = sizeof(AddressSpace) * 1024ull * 64;
        const uint64 VMAddressSpacePoolBase = VMRegionPoolStackBase - VMAddressSpacePoolLimit;
        const uint64 VMAddressSpacePoolStackLimit = 1024ull * 64;
        const uint64 VMAddressSpacePoolStackBase = VMAddressSpacePoolBase - VMAddressSpacePoolStackLimit;
        
        // core structures addresses and limits
        const uint64 CoreDataAreaBase = 0xFFFF800000000000;
        const uint64 CoreDataAreaLimit = 1024 * 1024 * 1024;
        const uint64 CorePageStackBase = 0xFFFF800040000000;
        const uint64 CorePageStackLimit = 1024 * 1024 * 1024;
    }
}

#endif

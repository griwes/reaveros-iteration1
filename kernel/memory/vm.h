#ifndef __rose_kernel_memory_vm_h__
#define __rose_kernel_memory_vm_h__

#include "../types.h"
#include "paging.h"
#include "../processor/synchronization.h"
#include "../processor/processor.h"
#include "../lib/vector.h"
#include "memory.h"

namespace Scheduler
{
    class Process;
}

class File;

namespace Memory
{
    namespace VM
    {
        // kernel global addresses and limits
        const uint64 PSPoolStackBase = 0xFFFFFD7D3F700000;
        const uint64 PSPoolStackEnd = 1025ull * 1024 * 1024;
        const uint64 HeapBase = 0xFFFFFD7D7F800000;
        const uint64 HeapLimit = 1024ull * 1024 * 1024 * 1024;
        const uint64 TCBPoolStackBase = 0xFFFFFE7D7F800000;
        const uint64 TCBPoolStackLimit = 4ull * 1024 * 1024;;
        const uint64 PCBPoolStackBase = 0xFFFFFE7D80000000;
        const uint64 PCBPoolStackLimit = 4ull * 1024 * 1024;;
        const uint64 PSPoolBase = 0xFFFFFF7DC0000000;
        const uint64 PSPoolLimit = 1025ull * 1024 * 1024 * 1024;
        const uint64 ACPIAreaBase = 0xFFFFFF7DC0000000;
        const uint64 ACPIAreaLimit = 2ull * 1024 * 1024 * 1024;
        const uint64 FreePageStackBase = 0xFFFFFF7E00000000;
        const uint64 FreePageStackLimit = 512ull * 1024 * 1024 * 1024;
        const uint64 PCBPoolBase = 0xFFFFFFFE00000000;
        const uint64 PCBPoolLimit = 2ull * 1024 * 1024 * 1024;
        const uint64 TCBPoolBase = 0xFFFFFFFE80000000;
        const uint64 TCBPoolLimit = 2ull * 1024 * 1024 * 1024;
        const uint64 VideoMemoryBase = 0xFFFFFFFF00000000;
        const uint64 VideoMemoryLimit = 1024ull * 1024 * 1024;
        const uint64 VideoBackbufferBase = 0xFFFFFFFF40000000;
        const uint64 VideoBackbufferLimit = 1024ull * 1024 * 1024;
        const uint64 KernelBase = 0xFFFFFFFF80000000;
        const uint64 KernelLimit = 2ull * 1024 * 1024 * 1024;

        // core structures addresses and limits
        const uint64 CoreDataAreaBase = 0xFFFF800000000000;
        const uint64 CoreDataAreaLimit = 1024 * 1024 * 1024;
        const uint64 CorePageStackBase = 0xFFFF800040000000;
        const uint64 CorePageStackLimit = 1024 * 1024 * 1024;
        
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

            Lib::Vector<Page *> Pages;

            uint64 Base;
            uint64 End;

            AddressSpace * Parent;
            AddressSpace * SharedAddressSpace;

            File * MappedFile;

            Processor::Spinlock Lock;
        };

        class AddressSpace
        {
        public:
            friend void Memory::RemapKernel();
            
            AddressSpace(uint64 = 0);
            ~AddressSpace();

            void AddRegion(uint64, uint64, Region * = nullptr);
            void AddRegion(Region *);
            void RemoveRegion(uint64);
            void RemoveRegion(Region *);
            
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
                    this->MapPage(start);
                    start += 4096;
                }
            }

            void MapPages(uint64 s, uint64 e, uint64 b)
            {
                m_lock.Lock();
                m_pPML4->Map(s, e, b);
                m_lock.Unlock();
            }

            void MapPage(Page *);

            void UnmapPage(uint64);
            void UnmapPages(uint64 start, uint64 end)
            {
                while (start < end)
                {
                    this->UnmapPage(start);
                    start += 4096;
                }
            }

            void SetActive()
            {
                Processor::LoadCR3(m_pPML4->GetPhysicalAddress((uint64)m_pPML4));
            }

            Scheduler::Process * Parent;
            Lib::Vector<Region *> Regions;

        private:
            Processor::Spinlock m_lock;
            Paging::PML4 * m_pPML4;
        };
    }
}

#endif
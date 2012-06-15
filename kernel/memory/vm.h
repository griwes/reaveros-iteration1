#ifndef __rose_kernel_memory_vm_h__
#define __rose_kernel_memory_vm_h__

#include "../types.h"
#include "paging.h"
#include "../processor/synchronization.h"
#include "../processor/processor.h"
#include "../lib/vector.h"

namespace Scheduler
{
    class Process;
}

class File;

namespace Memory
{
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
            AddressSpace();
            ~AddressSpace();

            void AddRegion(uint64, uint64);
            void AddRegion(Region *);
            void RemoveRegion(uint64);
            void RemoveRegion(Region *);
            
            void MapPage(uint64);
            void MapPages(uint64 start, uint64 end)
            {
                while (start < end)
                {
                    this->MapPage(start);
                    start += 4096;
                }
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
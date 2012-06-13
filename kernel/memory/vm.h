#ifndef __rose_kernel_memory_vm_h__
#define __rose_kernel_memory_vm_h__

#include "../types.h"
#include "paging.h"
#include "../processor/synchronization.h"
#include "../lib/list.h"

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

            uint64 PhysicalAddress;
            uint64 VirtualAddress;

            Region * Parent;

            Page * CoWBase;
            Lib::List<Page *> CoWCopies;
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

            Lib::List<Page *> Pages;

            uint64 VirtualAddress;
            uint64 Length;

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

            void AddRegion(Region *);
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

            Scheduler::Process * Parent;
            Lib::List<Region *> Regions;

        private:
            Processor::Spinlock m_lock;
        };
    }
}

#endif
#ifndef __rose_kernel_memory_vmm_h__
#define __rose_kernel_memory_vmm_h__

#include "../types.h"

namespace Memory
{
    namespace VMM
    {
        void * AllocPagingPages(uint64);

        void MapPage(uint64);
        void MapPage(uint64, uint64);
        void MapPages(uint64, uint64);
        void MapPages(uint64, uint64, uint64);
        void UnmapPage(uint64);
        void UnmapPages(uint64, uint64);
    }
}

#endif
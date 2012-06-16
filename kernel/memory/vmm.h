#ifndef __rose_kernel_memory_vmm_h__
#define __rose_kernel_memory_vmm_h__

#include "../types.h"
#include "memory.h"
#include "vm.h"

namespace Memory
{
    namespace VMM
    {        
        void * AllocPagingPages(uint64);

        inline void MapPage(uint64 p)
        {
            Memory::CurrentVAS->MapPage(p);
        }
        
        inline void MapPage(uint64 p, uint64 b)
        {
            Memory::CurrentVAS->MapPage(p, b);
        }
        
        inline void MapPages(uint64 s, uint64 e)
        {
            Memory::CurrentVAS->MapPages(s, e);
        }
        
        inline void MapPages(uint64 s, uint64 e, uint64 b)
        {
            Memory::CurrentVAS->MapPages(s, e, b);
        }
        
        inline void UnmapPage(uint64 p)
        {
            Memory::CurrentVAS->UnmapPage(p);
        }
        
        inline void UnmapPages(uint64 s, uint64 e)
        {
            Memory::CurrentVAS->UnmapPages(s, e);
        }
    }
}

#endif
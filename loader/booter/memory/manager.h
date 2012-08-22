#pragma once

#include <cstdint>
#include <cstddef>

#include "memmap.h"

namespace memory
{
    namespace manager
    {
        class allocator_t
        {
        public:
            allocator_t() {}
            virtual ~allocator_t() = 0;
            
            virtual void * allocate(uint32_t) = 0;
            virtual void deallocate(uint32_t) = 0;
        };
        
        class placement_allocator_t : public allocator_t
        {
        public:
            placement_allocator_t(uint32_t, map_t &);
            virtual ~placement_allocator_t();
            
            virtual void * allocate(uint32_t);
            virtual void deallocate(uint32_t);
        };
        
        allocator_t * make_placement_allocator(uint32_t, map_t &);
    }
}

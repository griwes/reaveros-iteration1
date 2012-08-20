#pragma once

#include <cstdint>
#include <cstddef>

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
        
        allocator_t * make_placement_allocator(uint32_t);
    }
}

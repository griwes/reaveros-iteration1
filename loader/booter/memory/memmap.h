#pragma once

#include <cstdint>
#include <cstddef>

#include "../processor/processor.h"

namespace memory
{
    class map_entry_t;
    
    class map_t
    {
    public:
        map_t(map_entry_t *, std::uint32_t);
        ~map_t();
        
        map_t * sanitize();
        void apply_numa(processor::numa_env_t *);
    };
}

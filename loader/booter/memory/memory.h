#pragma once

#include <cstdint>
#include <cstddef>

#include "memmap.h"

#include "../processor/processor.h"

namespace memory
{    
    namespace manager
    {
        class allocator_t;
    }
    
    manager::allocator_t * default_allocator;
    
    void initialize(uint32_t, map_t &);
    void prepare_long_mode();
}

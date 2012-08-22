#include "memory.h"
#include "manager.h"

namespace memory
{
    manager::allocator_t * default_allocator;
}

void memory::initialize(uint32_t placement, map_t & memory_map)
{
    default_allocator = manager::make_placement_allocator(placement, memory_map);
}

void memory::prepare_long_mode()
{
    
}

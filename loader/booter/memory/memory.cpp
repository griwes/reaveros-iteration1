#include "memory.h"
#include "manager.h"

void memory::initialize(uint32_t placement, map_t &)
{
    default_allocator = manager::make_placement_allocator(placement);
}

void memory::prepare_long_mode()
{
    
}

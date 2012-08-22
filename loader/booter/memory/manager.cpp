#include "manager.h"

void * operator new(uint32_t, void *);

memory::manager::allocator_t * memory::manager::make_placement_allocator(uint32_t placement, map_t & memory_map)
{
    placement += 15;
    placement &= ~(uint32_t)15;
    
    return new((void *)placement) placement_allocator_t((placement + sizeof(placement_allocator_t) + 15) & ~(uint32_t)15,
            memory_map);
}

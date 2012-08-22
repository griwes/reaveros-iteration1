#include "memmap.h"

memory::map_t::map_t(memory::map_entry_t * base_map, uint32_t map_size)
    : sequence_entries(base_map), entries(nullptr), num_entries(map_size)
{
}

memory::map_t::~map_t()
{

}

bool memory::map_t::usable(uint64_t addr, uint32_t domain)
{
    if (entries)
    {
        auto entry = entries;
        
        for (uint64_t i = 0; i < num_entries; i++)
        {
            if (addr >= entry->base && addr < entry->base + entry->length)
            {
                if (entry->type == 1)
                {
                    if (domain != ~(uint32_t)0)
                    {
                        if (entry->proximity_domain == domain)
                        {
                            return true;
                        }
                    
                        return false;
                    }
                
                    return true;
                }
                
                return false;
            }
        }
        
        return false;
    }
    
    else
    {
        auto entry = sequence_entries;
        
        for (uint64_t i = 0; i < num_entries; i++)
        {
            if (addr >= entry->base && addr < entry->base + entry->length)
            {
                if (entry->type == 1)
                {
                    return true;
                }
                
                return false;
            }
        }
        
        return false;
    }
}

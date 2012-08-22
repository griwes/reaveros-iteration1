#pragma once

#include "../processor/processor.h"

namespace acpi
{
    class rsdp_t
    {
    public:
        
    };
    
    rsdp_t * find_rsdp();
    
    processor::numa_env_t * find_numa_domains();
}

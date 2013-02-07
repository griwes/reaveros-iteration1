/**
 * Reaver Project OS, Rose License
 * 
 * Copyright (C) 2011-2013 Reaver Project Team:
 * 1. Michał "Griwes" Dominiak
 * 
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * 
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation is required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 * 
 * Michał "Griwes" Dominiak
 * 
 **/

#include <processor/smp.h>
#include <processor/current_core.h>
#include <processor/core.h>
#include <memory/memory.h>

extern "C" char trampoline_start[];
extern "C" char trampoline_end[];

void processor::smp::boot(core * cores, uint64_t num_cores)
{
    uint64_t trampoline_size = (uint64_t)(trampoline_end - trampoline_start);
    
    trampoline_size += 4095;
    trampoline_size &= ~(uint64_t)4095;
    
    for (uint64_t i = 0; i < num_cores; ++i)
    {
        memory::copy(trampoline_start + trampoline_size * i, 0x7c00 + trampoline_size * (i + 1), trampoline_size);
        *(uint64_t *)(trampoline_start + trampoline_size * i + 8) = &cores[i]->started;
    }
    
    // INIT IPI
    for (uint64_t i = 0; i < num_cores; ++i)
    {
        current_core::ipi(cores[i].apic_id(), current_core::ipis::init);
    }
    
    current_core::sleep(10000000);
    
    // SIPI
    for (uint64_t i = 0; i < num_cores; ++i)
    {
        current_core::ipi(cores[i].apic_id(), current_core::ipis::sipi, 0x7c00 + trampoline_size * i);
    }
    
    current_core::sleep(200000);
    
    // 2nd SIPI, if not started
    for (uint64_t i = 0; i < num_cores; ++i)
    {
        if (!cores[i].started)
        {
            current_core::ipi(cores[i].apic_id(), current_core::ipis::sipi, 0x7c00 + trampoline_size * i);
        }
    }
}

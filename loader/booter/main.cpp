/**
 * Reaver Project OS, Rose License
 * 
 * Copyright (C) 2011-2012 Reaver Project Team:
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

#include <cstdint>
#include <cstddef>

#include "processor/processor.h"
#include "screen/screen.h"
#include "acpi/acpi.h"
#include "memory/memory.h"
#include "memory/memmap.h"

extern "C" void __attribute__((cdecl)) booter_main(memory::map_entry * memory_map, uint32_t memory_map_size, 
                uint32_t placement, uint32_t /*kernel*/, uint32_t /*kernel_size*/, uint32_t /*initrd_size*/,
                screen::boot_mode * video_mode, void * font)
{
    memory::map mem_map(memory_map, memory_map_size);
    
    memory::initialize(placement, mem_map);
    screen::initialize(video_mode, font);
            
    screen::printl("Booter, Reaver Project Bootloader v0.3");
    screen::printl("Copyrights (C) 2012 Reaver Project Team");
    screen::line();
        
    screen::printl("[MEM ] Reading memory map...");
    screen::printl(mem_map);
    
    screen::print("[MEM ] Sanitizing memory map... ");
    memory::map * sane_map = mem_map.sanitize();
    screen::printl("done.");
    
    screen::printl("[MEM ] Printing sanitized memory map...");
    screen::printl(*sane_map);
  
    for (;;);
    
/*    screen::print("[CPU ] Checking CPU's long mode support... ");
    processor::check_long_mode();
    screen::printl("done.");
    
    screen::print("[MEM ] Preparing long mode paging... ");
    memory::prepare_long_mode();
    screen::printl("done.");
    
    screen::print("[CPU ] Entering long mode... ");
    processor::enter_long_mode();
    screen::printl("done.");
    
    screen::print("[CPU ] Installing long mode GDT... ");
    processor::setup_gdt();
    screen::printl("done.");
    
    screen::print("[ACPI] Looking for RSDP... ");
    acpi::rsdp * rsdp = acpi::find_rsdp();
    screen::printl("done.");
    
    screen::printl("[ACPI] Printing RSDP info...");
    screen::printl(rsdp);
    
    screen::print("[ACPI] Looking for NUMA domains... ");
    processor::numa_env * env = acpi::find_numa_domains();
    screen::printl("done.");
    
    screen::printl("[ACPI] Printing NUMA domain info...");
    screen::printl(*env);
    
    screen::print("[MEM ] Applying NUMA domains info to memory map... ");
    sane_map->apply_numa(env);
    screen::printl("done.");
    
    screen::printl("[MEM ] Printing NUMA-affected memory map... ");
    screen::printl(*sane_map);*/
}

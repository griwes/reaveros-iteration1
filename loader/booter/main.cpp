#include <cstdint>
#include <cstddef>

#include "processor/processor.h"
#include "screen/screen.h"
#include "acpi/acpi.h"
#include "memory/memory.h"
#include "memory/memmap.h"

extern "C" void __attribute__((cdecl)) booter_main(memory::map_entry_t * memory_map, uint32_t memory_map_size, 
                uint32_t placement, uint32_t /*kernel*/, uint32_t /*kernel_size*/, uint32_t /*initrd_size*/,
                screen::boot_mode_t * video_mode, void * font)
{
    memory::map_t mem_map(memory_map, memory_map_size);
    
    memory::initialize(placement, mem_map);
    screen::initialize(video_mode, font);
    
    screen::printl("Booter, Reaver Project Bootloader v0.3");
    screen::printl("Copyrights (C) 2012 Reaver Project Team");
    screen::line();
    
    screen::printl("[MEM ] Reading memory map...");
    screen::printl(mem_map, 2);
    
    screen::printl("[MEM ] Sanitizing memory map...");
    memory::map_t * sane_map = mem_map.sanitize();
    
    screen::printl("[MEM ] Printing sanitized memory map...");
    screen::printl(*sane_map);
    
    screen::print("[CPU ] Checking CPU's long mode support... ");
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
    
    screen::print("[ACPI] Looking for NUMA domains... ");
    processor::numa_env_t * env = acpi::find_numa_domains();
    screen::printl("done.");
    
    screen::printl("[ACPI] Printing NUMA domain info...");
    screen::printl(*env);
    
    screen::print("[MEM ] Applying NUMA domains info to memory map... ");
    sane_map->apply_numa(env);
    screen::printl("done.");
    
    screen::printl("[MEM ] Printing NUMA-affected memory map... ");
    screen::printl(*sane_map);
}

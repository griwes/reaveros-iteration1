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

#include <screen/screen.h>
#include <memory/memory.h>
#include <memory/pmm.h>

extern "C" void __attribute__((cdecl)) kernel_main(uint64_t /*initrd_start*/, uint64_t /*initrd_end*/, screen::mode * video,
    memory::map_entry * memory_map, uint64_t memory_map_size)
{
    memory::copy_bootloader_data(video, memory_map, memory_map_size);
    memory::initialize_paging();
    
    screen::initialize(video, memory_map, memory_map_size); // memory map required to get preallocated backbuffer info from bootloader
    
    screen::print("ReaverOS: Reaver Project Operating System \"Rose\"\n");
    screen::print("Version: 0.0.1, Codename \"Cotyledon\"\n");
    screen::print("Copyright (C) 2012-2013 Reaver Project Team\n\n");
    
    screen::print(tag::memory, "Initializing physical memory manager...");
    memory::pmm::initialize(memory_map, memory_map_size);
    screen::done();
    
    screen::print(tag::memory, "Reporting memory manager status...\n");
    memory::pmm::boot_report();
    
    for (;;) ;
    
/*    screen::print(tag::cpu, "Initializing processor...");
    processor::initialize();
    screen::done();
    
    screen::print(tag::scheduler, "Initializing scheduler...");
    scheduler::initialize();
    
    screen::print(tag::scheduler, "Initializing virtual memory manager server...");
    scheduler::process vmm = scheduler::create_process(initrd["vmm.srv"]);
    screen::done();
    
    screen::print(tag::scheduler, "Starting video server...");
    scheduler::process vsrv = scheduler::create_process(initrd["video.srv"]);
    screen::done();
    
    screen::print(tag::screen, "Switching to video server output...");
    screen::initialize_server(vsrv);
    screen::done();
    
    screen::print(tag::scheduler, "Starting device manager server...");
    scheduler::process device_mgr = scheduler::create_process(initrd["devicemgr.srv"]);
    screen::done();
    
    screen::print(tag::scheduler, "Starting VIOLA server...");
    scheduler::process viola = scheduler::create_process(initrd["viola.srv"]);
    screen::done();
    
    screen::print(tag::scheduler, "Starting storage device driver...");
    scheduler::process storage = scheduler::create_process(initrd["storage.srv"]);
    screen::done();
    
    screen::print(tag::scheduler, "Starting filesystem driver...");
    scheduler:: process filesystem = scheduler::create_process(initrd["filesystem.srv"]);
    screen::done();*/
}

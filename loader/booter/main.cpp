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

#include <processor/processor.h>
#include <screen/screen.h>
#include <memory/memory.h>
#include <memory/memmap.h>
#include <memory/x64paging.h>
#include <memory/manager.h>

extern "C" void __attribute__((cdecl)) booter_main(memory::map_entry * memory_map, uint32_t memory_map_size, 
                uint32_t kernel, uint32_t kernel_size, uint32_t initrd_size, screen::boot_mode * video_mode, void * font)
{    
    memory::initialize(kernel + kernel_size * 512 + initrd_size * 512, memory_map, memory_map_size);
    screen::initialize(video_mode, font);
    
    screen::printl("Booter, Reaver Project Bootloader 0.0.4");
    screen::printl("Copyright (C) 2012 Reaver Project Team");
    screen::line();
    
    screen::print("[CPU  ] Checking CPU's long mode support... ");
    processor::check_long_mode();
    screen::printl("done.");
    
    screen::printl("[VIDEO] Printing video mode details...");
    screen::output.print_mode_info();
    
    screen::printl("[MEM  ] Reading memory map...");
    memory::print_map();

    screen::print("[MEM  ] Preparing long mode paging... ");
    memory::prepare_long_mode();
    screen::printl("done.");
    
    screen::print("[CPU  ] Entering long mode... ");
    processor::enter_long_mode();
    memory::vas->map(screen::output.video_start(), screen::output.video_end(), screen::output.video_start());
    memory::vas->map(screen::output.backbuffer_start(), screen::output.backbuffer_end(), screen::output.backbuffer_start());
    screen::printl("done.");
    
    screen::print("[CPU  ] Installing long mode GDT... ");
    processor::setup_gdt();
    screen::printl("done.");

    screen::print("[CPU  ] Installing long mode IDT... ");
    processor::setup_idt();
    screen::printl("done.");
    
    memory::default_allocator.save();
    
    screen::print("[MEM  ] Installing kernel... ");
    uint64_t kernel_end = memory::install_kernel(kernel, kernel_size);
    memory::install_kernel_stack(kernel_end);
    screen::printl("done.");
    
    memory::default_allocator.save();
    
    screen::print("[MEM  ] Installing InitRD... ");
    memory::install_initrd(kernel_end, kernel + kernel_size + 5 * 4096, initrd_size);
    screen::printl("done.");
    
    memory::default_allocator.save();
    
    screen::printl("[MEM  ] Printing final memory map... ");
    memory::print_map();
    
    uint64_t kernel_start = 0xFFFFFFFF80000000;
    uint64_t initrd_start = kernel_end;
    uint64_t kernel_stack_end = kernel_end;
    
    screen::print("[CPU  ] Calling kernel...");
    processor::call_kernel(kernel_start, initrd_start, initrd_start + initrd_size, kernel_stack_end, 
        (uint64_t)screen::get_video_mode(), (uint64_t)memory::map::get_entries(), memory::map::size());
    
    // we will never get here
    for (;;) ;
    
    return;
}

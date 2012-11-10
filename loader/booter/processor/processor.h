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

#pragma once

#include <screen/screen.h>

namespace processor
{
    extern "C" struct idt_entry
    {
        uint16_t offset_low;
        uint16_t selector;
        uint8_t zero;
        uint8_t flags;
        uint16_t offset_middle;
        uint32_t offset_high;
        uint32_t zero1;
    } __attribute__((packed)) idt[256];
    
    struct isr_stack_frame
    {
        
    } __attribute__((packed));
        
    inline void set_cr3(uint32_t pd)
    {
        asm volatile ("movl %%eax, %%cr3" :: "a"(pd) : "memory");
    }
    
    inline void enable_paging()
    {
        uint32_t cr0;
        
        asm volatile ("movl %%cr0, %%eax" : "=a"(cr0));
        
        cr0 |= 0x80000000;
        
        asm volatile ("movl %%eax, %%cr0" :: "a"(cr0) : "memory");
    }
    
    inline void disable_paging()
    {
        uint32_t cr0;
        
        asm volatile ("movl %%cr0, %%eax" : "=a"(cr0));
        
        cr0 &= ~0x80000000;
        
        asm volatile ("movl %%eax, %%cr0" :: "a"(cr0) : "memory");
    }
    
    extern "C" uint32_t _check_long_mode();
    
    inline void check_long_mode()
    {
        if (!_check_long_mode())
        {
            screen::printl("failed.");
            screen::line();
            screen::printl("This software cannot be executed on your PC, because your CPU does not support long mode.");
            screen::printl("To run this software, upgrade your CPU to one supporting long mode (so called \"64-bit CPUs\").");
            
            asm volatile ("hlt");
        }
    }
    
    inline void invlpg(uint64_t address)
    {
        asm volatile ("invlpg (%0)" :: "r"(address) : "memory");
    }
    
    extern "C" void enter_long_mode();
    extern "C" void setup_gdt();
    void setup_idt();
}

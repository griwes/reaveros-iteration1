/**
 * Reaver Project OS, Rose License
 * 
 * Copyright (C) 2012 Reaver Project Team:
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

namespace processor
{
    idt_entry idt[256];
}

extern "C" void _setup_idt();

extern "C" char isr0[];
extern "C" char isr1[];
extern "C" char isr2[];
extern "C" char isr3[];
extern "C" char isr4[];
extern "C" char isr5[];
extern "C" char isr6[];
extern "C" char isr7[];
extern "C" char isr8[];
extern "C" char isr9[];
extern "C" char isr10[];
extern "C" char isr11[];
extern "C" char isr12[];
extern "C" char isr13[];
extern "C" char isr14[];
extern "C" char isr15[];
extern "C" char isr16[];
extern "C" char isr17[];
extern "C" char isr18[];
extern "C" char isr19[];
extern "C" char isr20[];
extern "C" char isr21[];
extern "C" char isr22[];
extern "C" char isr23[];
extern "C" char isr24[];
extern "C" char isr25[];
extern "C" char isr26[];
extern "C" char isr27[];
extern "C" char isr28[];
extern "C" char isr29[];
extern "C" char isr30[];
extern "C" char isr31[];

namespace processor
{
    struct idtr
    {
        uint16_t limit;
        uint64_t base;
    } __attribute__((packed));
}

extern "C" processor::idtr idtr;
processor::idtr idtr;

namespace
{    
    void _setup_idte(uint8_t id, uint64_t address, uint16_t selector, uint8_t flags)
    {
        processor::idt[id].zero = 0;
        processor::idt[id].zero1 = 0;
        processor::idt[id].offset_low = address & 0xffff;
        processor::idt[id].offset_middle = (address >> 16) & 0xffff;
        processor::idt[id].offset_high = (address >> 32) & 0xffffffff;
        processor::idt[id].selector = selector;
        processor::idt[id].flags = flags;
    }
}

void processor::setup_idt()
{    
    _setup_idte(0, (uint64_t)isr0, 0x08, 0x8e);
    _setup_idte(1, (uint64_t)isr1, 0x08, 0x8e);
    _setup_idte(2, (uint64_t)isr2, 0x08, 0x8e);
    _setup_idte(3, (uint64_t)isr3, 0x08, 0x8e);
    _setup_idte(4, (uint64_t)isr4, 0x08, 0x8e);
    _setup_idte(5, (uint64_t)isr5, 0x08, 0x8e);
    _setup_idte(6, (uint64_t)isr6, 0x08, 0x8e);
    _setup_idte(7, (uint64_t)isr7, 0x08, 0x8e);
    _setup_idte(8, (uint64_t)isr8, 0x08, 0x8e);
    _setup_idte(9, (uint64_t)isr9, 0x08, 0x8e);
    _setup_idte(10, (uint64_t)isr10, 0x08, 0x8e);
    _setup_idte(11, (uint64_t)isr11, 0x08, 0x8e);
    _setup_idte(12, (uint64_t)isr12, 0x08, 0x8e);
    _setup_idte(13, (uint64_t)isr13, 0x08, 0x8e);
    _setup_idte(14, (uint64_t)isr14, 0x08, 0x8e);
    _setup_idte(15, (uint64_t)isr15, 0x08, 0x8e);
    _setup_idte(16, (uint64_t)isr16, 0x08, 0x8e);
    _setup_idte(17, (uint64_t)isr17, 0x08, 0x8e);
    _setup_idte(18, (uint64_t)isr18, 0x08, 0x8e);
    _setup_idte(19, (uint64_t)isr19, 0x08, 0x8e);
    _setup_idte(20, (uint64_t)isr20, 0x08, 0x8e);
    _setup_idte(21, (uint64_t)isr21, 0x08, 0x8e);
    _setup_idte(22, (uint64_t)isr22, 0x08, 0x8e);
    _setup_idte(23, (uint64_t)isr23, 0x08, 0x8e);
    _setup_idte(24, (uint64_t)isr24, 0x08, 0x8e);
    _setup_idte(25, (uint64_t)isr25, 0x08, 0x8e);
    _setup_idte(26, (uint64_t)isr26, 0x08, 0x8e);
    _setup_idte(27, (uint64_t)isr27, 0x08, 0x8e);
    _setup_idte(28, (uint64_t)isr28, 0x08, 0x8e);
    _setup_idte(29, (uint64_t)isr29, 0x08, 0x8e);
    _setup_idte(30, (uint64_t)isr30, 0x08, 0x8e);
    _setup_idte(31, (uint64_t)isr31, 0x08, 0x8e);
    
    ::idtr.limit = 32 * 16 - 1;
    ::idtr.base = (uint64_t)idt;
    
    _setup_idt();
}

namespace
{
    const char * exception_messages[] = 
    {
        "#DE: Divide error",
        "#DB: Reserved",
        "###: Non Maskable Interrupt",
        "#BP: Breakpoint",
        "#OF: Overflow",
        "#BR: BOUND Range Exceeded",
        "#UD: Invalid Opcode",
        "#NM: No Math Coprocessor",
        "#DF: Double Fault",
        "###: Coprocessor Segment Overrun (reserved)",
        "#TS: Invalid TSS",
        "#NP: Segment Not Present",
        "#SS: Stack Segment Fault",
        "#GP: General Protection Fault",
        "#PF: Page Fault",
        "###: Reserved",
        "#MF: x87 FPU Error",
        "#AC: Alignment Check",
        "#MC: Machine Check",
        "#XM: SIMD Floating-Point Exception",
        "###: Reserved",
        "###: Reserved",
        "###: Reserved",
        "###: Reserved",
        "###: Reserved",
        "###: Reserved",
        "###: Reserved",
        "###: Reserved",
        "###: Reserved",
        "###: Reserved",
        "###: Reserved",
        "###: Reserved"
    };
}

extern "C" void isr_handler(processor::isr_stack_frame stack_frame)
{
    screen::line();
    screen::printl("ISR#", (uint8_t)stack_frame.number, ": ", (const char *)exception_messages[stack_frame.number]);
    screen::printfl("RAX = 0x%016x RBX = 0x%016x\nRCX = 0x%016x RDX = 0x%016x", stack_frame.rax, stack_frame.rbx,
                   stack_frame.rcx, stack_frame.rdx);
    screen::printfl("RSI = 0x%016x RDI = 0x%016x\nCS  = 0x%016x RIP = 0x%016x", stack_frame.rsi, stack_frame.rdi,
                   stack_frame.cs, stack_frame.rip);
    screen::printfl("SS  = 0x%016x RSP = 0x%016x", stack_frame.ss, stack_frame.rsp);
    
    // TODO: error code handling, cr2 handling for page faults
    
    asm volatile ("cli; hlt");
}

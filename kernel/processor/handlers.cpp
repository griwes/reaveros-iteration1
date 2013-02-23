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

#include <processor/handlers.h>
#include <screen/console.h>
#include <screen/screen.h>
#include <processor/current_core.h>
#include <processor/processor.h>
#include <processor/ioapic.h>

namespace
{
    processor::interrupts::handler _irq_handlers[224] = {};
}

// TODO: proper handling of errors, better screen of death with printing faulting address, registers and stuff

void processor::exceptions::reserved(processor::idt::exc_context ctx)
{
    if (ctx.cs & 3)
    {
        // resolve problem by - probably - killing a process

        return;
    }

    PANIC("Reserved exception fired");

    return;
}

void processor::exceptions::divide_error(processor::idt::exc_context ctx)
{
    if (ctx.cs & 3)
    {
        // resolve problem by killing a process

        return;
    }

    PANIC("Divide by zero exception in kernel code");

    return;
}

void processor::exceptions::bound_range(processor::idt::exc_context ctx)
{
    if (ctx.cs & 3)
    {
        // signal the process with rose::signal::bound_exceeded

        return;
    }

    PANIC("Bound range exceeded in kernel code");

    return;
}

void processor::exceptions::invalid_opcode(processor::idt::exc_context ctx)
{
    if (ctx.cs & 3)
    {
        // kill the process

        return;
    }

    PANIC("Invalid opcode in kernel code");

    return;
}

void processor::exceptions::no_coprocessor(processor::idt::exc_context)
{
    PANIC("FPU not found");

    return;
}

void processor::exceptions::invalid_tss(processor::idt::exc_context_error)
{
    PANIC("Invalid TSS exception");

    return;
}

void processor::exceptions::segment_not_present(processor::idt::exc_context_error)
{
    PANIC("Segment not present exception");

    return;
}

void processor::exceptions::stack_fault(processor::idt::exc_context_error ctx)
{
    if (ctx.cs & 3)
    {
        // kill the process

        return;
    }

    PANIC("Stack segment fault in kernel code");

    return;
}

void processor::exceptions::protection_fault(processor::idt::exc_context_error ctx)
{
    if (ctx.cs & 3)
    {
        // kill the process

        return;
    }

    PANIC("General protection fault in kernel code");

    return;
}

void processor::exceptions::page_fault(processor::idt::exc_context_error ctx)
{
    if (ctx.cs & 3)
    {
        // inform VMM

        return;
    }

    // TODO: #PF logic for sparse arrays

    void * addr = nullptr;

    asm volatile ("mov %%cr2, %0" : "=r"(addr));

    uint64_t address = (uint64_t)addr;
    if (address > memory::vm::stack_area && address <= memory::vm::stack_stack_area)
    {
        if (((address + 4095) & ~(uint64_t)4095) % 4096)
        {
            screen::transaction();
            screen::print("\nAt: ", addr);

            PANIC("Kernel stack overflow");
        }

        else
        {
            memory::vm::map(address % 4096 ? address : address - 1);
        }
    }

    screen::transaction();
    screen::print("\n#PF address: ", addr);

    PANIC("Page fault in kernel code");
}

void processor::exceptions::fpu_error(processor::idt::exc_context ctx)
{
    if (ctx.cs & 3)
    {
        // kill the process

        return;
    }

    PANIC("FPU error in kernel code");

    return;
}

void processor::exceptions::alignment_check(processor::idt::exc_context)
{
    PANIC("Unexpected alignment check exception");

    return;
}

void processor::exceptions::simd_exception(processor::idt::exc_context ctx)
{
    if (ctx.cs & 3)
    {
        // kill the process

        return;
    }

    PANIC("SIMD exception in kernel code");

    return;
}

void processor::exceptions::breakpoint(processor::idt::exc_context)
{
    PANIC("Unexpected breakpoint exception");

    return;
}

void processor::exceptions::overflow(processor::idt::exc_context ctx)
{
    if (ctx.cs & 3)
    {
        // kill the process

        return;
    }

    PANIC("Overflow in kernel code");

    return;
}

void processor::exceptions::double_fault(processor::idt::exc_context_error)
{
    PANIC("Double fault");

    return;
}

void processor::exceptions::machine_check(processor::idt::exc_context)
{
    PANIC("Machine check exception");

    return;
}

void processor::exceptions::non_maskable(processor::idt::exc_context)
{
    PANIC("Non maskable interrupt");
}

void processor::interrupts::common_interrupt_handler(processor::idt::irq_context ctx)
{
    processor::current_core::eoi(ctx.number);

    if (_irq_handlers[ctx.number - 32])
    {
        _irq_handlers[ctx.number - 32](ctx);
    }

    else
    {
        idt::disable(ctx.number);
        // scheduler::interrupt(ctx);
    }
}

void processor::interrupts::set_handler(uint8_t vector, processor::interrupts::handler handler)
{
    if (_irq_handlers[vector - 32] != nullptr)
    {
        PANIC("Tried to overwrite already existing IRQ handler");
    }

    _irq_handlers[vector - 32] = handler;

    idt::enable(vector);
}

void processor::interrupts::remove_handler(uint8_t vector)
{
    idt::disable(vector);

    _irq_handlers[vector - 32] = nullptr;
}

namespace
{
    bool _handlers[224] = { false };
}

uint8_t processor::interrupts::allocate(processor::interrupts::handler h, uint8_t priority)
{
    _handlers[0xFF - 32] = true;

    if (priority < 2)
    {
        PANIC("Exception priority interrupt vector requested");
    }

    for (uint8_t current_priority = priority; current_priority > 1; --current_priority)
    {
        for (uint8_t i = current_priority * 16; i < (current_priority + 1) * 16; ++i)
        {
            if (!_handlers[i - 32])
            {
                _handlers[i - 32] = true;

                interrupts::set_handler(i, h);

                return i;
            }
        }
    }

    return allocate(h, priority + 1);

    if (priority == 0xF)
    {
        PANIC("All interrupt numbers exhausted");
    }

    return 0;
}

void processor::interrupts::free(uint8_t idx)
{
    if (_handlers[idx - 32])
    {
        _handlers[idx - 32] = false;

        interrupts::remove_handler(idx);
    }

    else
    {
        PANIC("Tried to free not allocated interrupt handler");
    }
}

void processor::interrupts::set_isa_irq_int_vector(uint8_t isa, uint8_t handler)
{
    processor::get_ioapic(processor::translate_isa(isa)).route_interrupt(isa, handler);
}

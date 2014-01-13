/**
 * Reaver Project OS, Rose License
 *
 * Copyright © 2013-2014 Michał "Griwes" Dominiak
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
 **/

#include <processor/handlers.h>
#include <utils/spinlock.h>
#include <screen/screen.h>
#include <processor/ioapic.h>
#include <processor/lapic.h>
#include <processor/core.h>
#include <devices/devices.h>
#include <scheduler/scheduler.h>
#include <scheduler/thread.h>

namespace
{
    utils::spinlock _lock;

    using _handler = void (*)(processor::isr_context &, uint64_t);
    _handler _handlers[256] = {};
    uint64_t _contexts[256] = {};

    bool _vector_allocated[224] = {};
    devices::device * _owners[224] = {};

    void _print_registers(processor::isr_context & ctx)
    {
        screen::print("Registers:\n");
        screen::print("rax: ", (void *)ctx.rax, ", rbx: ", (void *)ctx.rbx, "\n");
        screen::print("rcx: ", (void *)ctx.rcx, ", rdx: ", (void *)ctx.rdx, "\n");
        screen::print("rsi: ", (void *)ctx.rsi, ", rdi: ", (void *)ctx.rdi, "\n");
        screen::print("rsp: ", (void *)ctx.rsp, ", rbp: ", (void *)ctx.rbp, "\n");
        screen::print("r8:  ", (void *)ctx.r8, ", r9:  ", (void *)ctx.r9, "\n");
        screen::print("r10: ", (void *)ctx.r10, ", r11: ", (void *)ctx.r11, "\n");
        screen::print("r12: ", (void *)ctx.r12, ", r13: ", (void *)ctx.r13, "\n");
        screen::print("r14: ", (void *)ctx.r14, ", r15: ", (void *)ctx.r15, "\n");
    }

    void _page_fault(processor::isr_context & context, uint64_t)
    {
        if ((context.cs & 3) != 0)
        {
            TODO; // sigsegv, i.e. stub signals
        }

        uint64_t cr2 = 0;

        asm volatile ("mov %%cr2, %%rax" : "=a"(cr2));

        PANICEX("Page fault in kernel code.", [&]
        {
            screen::print("Fault address: ", (void *)cr2, "\n");
            screen::print("Instruction pointer: ", (void *)context.rip, "\n");
            screen::print("Error cause: ", context.error & 1 ? "present" : "non-present", ", ");
            screen::print(context.error & 2 ? "write" : "read", "");
            screen::print(context.error & (1 << 3) ? ", reserved bit violation" : "");
            screen::print(context.error & (1 << 4) ? ", instruction fetch\n" : "\n");
            _print_registers(context);
        });
    }
}

void processor::initialize_exceptions()
{
    _handlers[14] = _page_fault;
}

void processor::handle(processor::isr_context & context)
{
    processor::get_current_core()->_is_in_interrupt = true;

    uint64_t tid = 0;
    scheduler::thread * interrupted_thread = nullptr;

    if (likely(scheduler::ready()))
    {
        interrupted_thread = scheduler::current_thread();
        tid = interrupted_thread->id;
    }

    uint64_t c = _contexts[context.number];
    _handler handler = _handlers[context.number];

    if (handler)
    {
        handler(context, c);
    }

    if (context.number >= 32)
    {
        if (_owners[context.number - 32])
        {
            _owners[context.number - 32]->mask_vector(context.number);
        }

        get_lapic()->eoi(context.number);
    }

    else if ((context.cs & 3) == 0 && !handler)
    {
        PANICEX("Unhandled CPU exception.", [&]
        {
            screen::print("Exception vector: ", context.number, "\n");
            screen::print("Error code: ", context.error, "\n");
            screen::print("Instruction pointer: ", (void *)context.rip, "\n");
            _print_registers(context);
        });
    }

    if (tid && scheduler::current_thread()->id != tid)
    {
        if (scheduler::valid(interrupted_thread))
        {
            interrupted_thread->save(context);
        }

        scheduler::current_thread()->load(context);
    }

    processor::get_current_core()->_is_in_interrupt = false;
}

uint8_t processor::allocate_isr(uint8_t priority, devices::device * owner)
{
    uint8_t i = 1;
    return allocate_isr(priority, i, owner);
}

uint8_t processor::allocate_isr(uint8_t priority, uint8_t & count, devices::device * owner)
{
    screen::debug("\nTrying to allocate ", count, " interrupt vector", count != 1 ? "s" : "", " at priority ", priority);

    if (count == 0)
    {
        PANIC("Invalid request to allocate 0 interrupt numbers.");
    }

    INTL();
    LOCK(_lock);

    for (uint8_t ret = (priority * 16 + count - 1) & ~(count - 1);; ret -= count)
    {
        bool good = true;

        for (uint8_t i = 0; i < count; ++i)
        {
            if (_vector_allocated[ret + i])
            {
                good = false;
                break;
            }
        }

        if (good)
        {
            for (uint8_t i = 0; i < count; ++i)
            {
                _vector_allocated[ret + i] = true;
                _owners[ret + i] = owner;
            }

            screen::debug("\nAllocated ", count, " interrupt vector", count != 1 ? "s starting" : "", " at ", ret + 32);

            return ret + 32;
        }

        if (ret == 0)
        {
            break;
        }
    }

    for (uint8_t ret = (priority * 16 + count - 1) & ~(count - 1); ret < 224; ret += count)
    {
        bool good = true;

        for (uint8_t i = 0; i < count; ++i)
        {
            if (_vector_allocated[ret + i])
            {
                good = false;
                break;
            }
        }

        if (good)
        {
            for (uint8_t i = 0; i < count; ++i)
            {
                _vector_allocated[ret + i] = true;
                _owners[ret + i] = owner;
            }

            screen::debug("\nAllocated ", count, " interrupt vector", count != 1 ? "s starting" : "", " at ", ret + 32);

            return ret + 32;
        }
    }

    if (count != 1)
    {
        count /= 2;
        return allocate_isr(priority, count);
    }

    PANIC("Interrupt allocation failed.");
}

void processor::free_isr(uint8_t number)
{
    INTL();
    LOCK(_lock);

    if (!_vector_allocated[number - 32])
    {
        PANIC("Tried to free a non-allocated interrupt number.");
    }

    _vector_allocated[number - 32] = false;
    _owners[number - 32] = nullptr;
}

void processor::register_handler(uint8_t number, _handler handler, uint64_t context)
{
    INTL();
    LOCK(_lock);

    if (_handlers[number])
    {
        PANIC("Tried to register handler for already registered interrupt handler.");
    }

    _handlers[number] = handler;
    _contexts[number] = context;
}

void processor::unregister_handler(uint8_t number)
{
    INTL();
    LOCK(_lock);

    if (!_handlers[number])
    {
        PANIC("Tried to unregister not registered interrupt handler.");
    }

    _handlers[number] = nullptr;
    _contexts[number] = 0;
}

void processor::set_isa_irq_int_vector(uint8_t isa, uint8_t handler)
{
    processor::get_ioapic(processor::translate_isa(isa))->route_interrupt(isa, handler);
}

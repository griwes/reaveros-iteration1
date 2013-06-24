/**
 * Reaver Project OS, Rose License
 *
 * Copyright (C) 2013 Reaver Project Team:
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
#include <utils/spinlock.h>
#include <screen/screen.h>

namespace
{
    utils::spinlock _lock;

    using _handler = void (*)(processor::idt::isr_context);
    _handler _handlers[256] = {};

    uint8_t _vector_allocated[224] = {};

    void _page_fault(processor::idt::isr_context context)
    {
        if ((context.cs & 3) != 0)
        {
            return;
        }

        uint64_t cr2 = 0;

        asm volatile ("mov %%cr2, %%rax" : "=a"(cr2));

        PANICEX("Page fault in kernel code.", [&]
        {
            screen::print("Fault address: ", (void *)cr2, "\n");
            screen::print("Instruction pointer: ", (void *)context.rip, "\n");
            screen::print("Error cause: ", context.error & 1 ? "present" : "non-present", ", ");
            screen::print(context.error & 2 ? "write" : "read", ", ");
            screen::print(context.error & (1 << 3) ? "reserved bit violation" : "");
            screen::print(context.error & (1 << 4) ? ", instruction fetch\n" : "\n");
        });
    }
}

void processor::initialize_exceptions()
{
    _handlers[14] = _page_fault;
}

void processor::handle(processor::idt::isr_context context)
{
    _handler handler = _handlers[context.number];

    if (handler)
    {
        handler(context);

        return;
    }

    if (context.number < 32 && (context.cs & 3) == 0)
    {
        PANICEX("Unhandled CPU exception.", [&]
        {
            screen::print("Exception vector: ", context.number, "\n");
            screen::print("Error code: ", context.error, "\n");
            screen::print("Instruction pointer: ", (void *)context.rip, "\n");
        });
    }
}

uint8_t processor::allocate_isr(uint8_t priority, uint8_t count)
{
    auto _ = utils::make_unique_lock(_lock);

    uint64_t min_sum = ~0ull;

    for (uint8_t ret = (priority * 8 + count - 1) & ~(count - 1), penalty = 0; ret < 224; ret += count, penalty += count)
    {
        uint64_t sum = penalty;

        for (uint8_t i = 0; i < ret; ++i)
        {
            if (_vector_allocated[ret + i] == 255)
            {
                goto skip1;
            }

            sum += _vector_allocated[ret + i];
        }

        if (sum == 0)
        {
            for (uint8_t i = 0; i < ret; ++i)
            {
                ++_vector_allocated[ret + i];
            }

            return ret - 32;
        }

        min_sum = sum < min_sum ? sum : min_sum;

    skip1:
        continue;
    }

    if (min_sum == ~0ull)
    {
        PANIC("Interrupt number allocation algorithm failed hard. File a bug request with full machine specs attached.");
    }

    for (uint8_t ret = (priority * 8 + count - 1) & ~(count - 1), penalty = 0; ret < 224; ret += count, penalty += count)
    {
        uint64_t sum = penalty;

        for (uint8_t i = 0; i < ret; ++i)
        {
            if (_vector_allocated[ret + i] == 255)
            {
                goto skip2;
            }

            sum += _vector_allocated[ret + i];
        }

        if (sum == min_sum)
        {
            for (uint8_t i = 0; i < ret; ++i)
            {
                ++_vector_allocated[ret + i];
            }

            return ret - 32;
        }

    skip2:
        continue;
    }

    __builtin_unreachable();
}

void processor::free_isr(uint8_t number)
{
    auto _ = utils::make_unique_lock(_lock);

    if (!_vector_allocated[number - 32])
    {
        PANIC("Tried to free a non-allocated interrupt number.");
    }

    --_vector_allocated[number - 32];
}

void processor::register_handler(uint8_t number, _handler handler)
{
    auto _ = utils::make_unique_lock(_lock);

    if (_handlers[number])
    {
        PANIC("Tried to register handler for already registered interrupt handler.");
    }

    _handlers[number] = handler;
}

void processor::unregister_handler(uint8_t number)
{
    auto _ = utils::make_unique_lock(_lock);

    if (!_handlers[number])
    {
        PANIC("Tried to unregister not registered interrupt handler.");
    }

    _handlers[number] = nullptr;
}

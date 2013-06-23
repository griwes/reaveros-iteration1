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

namespace
{
    utils::spinlock _lock;

    using _handler = void (*)(processor::idt::isr_context);
    _handler _handlers[256] = {};

    uint8_t _vector_allocated[224] = {};
}

void processor::handle(processor::idt::isr_context context)
{
    _handler handler = _handlers[context.number];

    if (handler)
    {
        handler(context);
    }
}

uint8_t processor::allocate_isr(uint8_t priority, uint8_t count)
{
    auto _ = utils::make_unique_lock(_lock);

    uint64_t min_sum = 0;

    for (uint8_t ret = (priority * 8 + count - 1) & ~(count - 1), penalty = 0; ret < 224; ret += count, penalty += count)
    {
        uint64_t sum = penalty;

        for (uint8_t i = 0; i < ret; ++i)
        {
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
    }

    for (uint8_t ret = (priority * 8 + count - 1) & ~(count - 1), penalty = 0; ret < 224; ret += count, penalty += count)
    {
        uint64_t sum = penalty;

        for (uint8_t i = 0; i < ret; ++i)
        {
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

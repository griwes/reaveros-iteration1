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

#include <memory/stack.h>
#include <memory/map.h>

memory::pmm::frame_stack::frame_stack(memory::map_entry * map, uint64_t map_size)
{
    for (uint64_t i = 0; i < map_size; ++i)
    {
        if (map[i].type == 1 && (map[i].base >= 1024 * 1024 || map[i].base + map[i].length > 1024 * 1024))
        {
            for (uint64_t frame = (map[i].base < 1024 * 1024) ? (1024 * 1024) : ((map[i].base + 4095) & ~(uint64_t)4095);
                frame < map[i].base + map[i].length; frame += 4096)
            {
                push(frame);
            }
        }
    }
}

uint64_t memory::pmm::frame_stack::pop()
{
    if (!_size)
    {
        PANIC("TODO: _size = 0");

        if (_first)
        {
            // dealocate chunk
        }

        else
        {
            if (_global)
            {
                push_chunk(_global->pop_chunk());
            }

            else
            {
                // free caches etc.
            }
        }
    }

    auto _ = utils::make_unique_lock(_last->lock);

    uint64_t ret = _last->stack[_last->size--];

    if (_last->size == 0)
    {
        auto __ = utils::make_unique_lock(_last->prev->lock);

        _last = _last->prev;

        if (_last->next->next)
        {
            // free _last->next->next
        }
    }

    return ret;
}

memory::pmm::frame_stack_chunk * memory::pmm::frame_stack::pop_chunk()
{
    auto _ = utils::make_unique_lock(_first->lock);

    if (_first->next)
    {
        auto _ = utils::make_unique_lock(_first->next->lock);
    }

    auto ret = _first;
    _first = _first->next;

    return ret;
}

void memory::pmm::frame_stack::push(uint64_t frame)
{
    if (!_first)
    {
        _first = allocate_chained<frame_stack_chunk>();

        _last = _first;
    }

    if (_last->size == frame_stack_chunk::max - 100 && !_last->next)
    {
        auto _ = utils::make_unique_lock(_last->lock);

        _last->next = allocate_chained<frame_stack_chunk>();
        _last->next->prev = _last;
    }

    auto _ = utils::make_unique_lock(_last->lock);

    _last->stack[_last->size++] = frame;
    ++_size;
}

void memory::pmm::frame_stack::push_chunk(memory::pmm::frame_stack_chunk * chunk)
{
    auto _ = utils::make_unique_lock(_last->lock);
    auto __ = utils::make_unique_lock(chunk->lock);

    auto last = _last;

    while (last->next)
    {
        last = last->next;
    }

    auto ___ = utils::make_unique_lock(last->lock);

    last->next = chunk;
}

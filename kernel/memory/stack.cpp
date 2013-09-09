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

#include <screen/screen.h>

extern memory::pmm::frame_stack _global_stack;

memory::pmm::frame_stack::frame_stack() : _first{}, _last{}, _size{}, _global{ &_global_stack }
{
}

memory::pmm::frame_stack::frame_stack(memory::map_entry * map, uint64_t map_size) : _first{}, _last{}, _size{}, _global{}
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
        if (_global)
        {
            push_chunk(_global->pop_chunk());
        }

        else
        {
            if (_first)
            {
                PANIC("TODO: deallocate available chunk");
            }

            else
            {
                PANIC("TODO: ultimate OOM case");
            }
        }
    }

    LOCK(_last->lock);

    uint64_t ret = _last->stack[--_last->size];

    if (_last->size == 0)
    {
        if (_first != _last)
        {
            _last = _last->prev;
        }

        else
        {
            PANIC("TODO: frame stack exhausted");
        }
    }

    if (_last != _first && _last->size == frame_stack_chunk::max - 50)
    {
        LOCK(_last->lock);
        LOCK(_last->next->lock);

        if (_last->next->next)
        {
            // free _last->next->next
        }
    }

    --_size;

    return ret;
}

memory::pmm::frame_stack_chunk * memory::pmm::frame_stack::pop_chunk()
{
    if (_first == _last)
    {
        return nullptr;
    }

    LOCK(_first->lock);

    if (_first->next)
    {
        LOCK(_first->next->lock);

        _first->next->prev = nullptr;
    }

    auto ret = _first;
    _first = _first->next;

    _size -= ret->size;

    return ret;
}

void memory::pmm::frame_stack::push(uint64_t frame)
{
    {
        LOCK(_lock);

        if (!_first)
        {
            if (!_first)
            {
                _last = _first = allocate_chained<frame_stack_chunk>();
            }
        }

        if (_last->size == frame_stack_chunk::max - 100 && !_last->next)
        {
            LOCK(_last->lock);

            _last->next = allocate_chained<frame_stack_chunk>();
            _last->next->prev = _last;
        }

        {
            LOCK(_last->lock);

            if (_last->size == frame_stack_chunk::max)
            {
                _last = _last->next;
            }
        }
    }

    LOCK(_last->lock);

    _last->stack[_last->size++] = frame;
    ++_size;
}

void memory::pmm::frame_stack::push_chunk(memory::pmm::frame_stack_chunk * chunk)
{
    if (chunk->size == frame_stack_chunk::max)
    {
        LOCK(_first->lock);

        chunk->prev = nullptr;
        chunk->next = _first;
        _first = chunk;

        return;
    }

    LOCK(_last->lock);
    LOCK(chunk->lock);

    auto last = _last;

    while (last->next)
    {
        last = last->next;
    }

    LOCK(last->lock);

    last->next = chunk;
    chunk->prev = last;
    chunk->next = nullptr;

    _size += chunk->size;
}

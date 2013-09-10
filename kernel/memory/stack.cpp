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
#include <processor/ipi.h>
#include <processor/core.h>

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
            if (_global->size() <= frame_stack_chunk::max)
            {
                processor::smp::parallel_execute(processor::smp::policies::others, [](uint64_t){
                    for (uint64_t i = 0; i < 16 && processor::get_current_core()->frame_stack().size() > frame_stack_chunk::max * 128;
                        ++i)
                    {
                        screen::debug("\nRebalancing: pushing frame stack chunk to global from #", processor::id());
                        _global_stack.push_chunk(processor::get_current_core()->frame_stack().pop_chunk());
                    }
                });
            }

            for (uint64_t i = 0; i < 8 && _global->size() > frame_stack_chunk::max; ++i)
            {
                screen::debug("\nRebalancing: pushing frame stack chunk from global to #", processor::id());
                push_chunk(_global->pop_chunk());
            }
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
    --_size;

    if (_last != _first && _last->size == frame_stack_chunk::max - 50)
    {
        LOCK(_last->lock);
        LOCK(_last->next->lock);

        if (_last->next->next)
        {
            // free _last->next->next
        }
    }

    screen::debug("\nPopped ", (void *)ret, " from ", (_global ? "local" : "global"), " frame stack on #", processor::id());

    return ret;
}

memory::pmm::frame_stack_chunk * memory::pmm::frame_stack::pop_chunk()
{
    if (_first == _last)
    {
        PANIC("tried to pop chunk from empty frame stack!");
    }

    LOCK(_first->lock);

    if (_first->next)
    {
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
            _last = _first = allocate_chained<frame_stack_chunk>(frame);

            return;
        }

        if (_last->size == frame_stack_chunk::max - 100 && !_last->next)
        {
            LOCK(_last->lock);

            _last->next = allocate_chained<frame_stack_chunk>(frame);
            _last->next->prev = _last;

            return;
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

    LOCK(_lock);

    if (_global && _size > _balance)
    {
        screen::debug("\nRebalancing: pushing frame stack chunk to global from #", processor::id());
        _global_stack.push_chunk(pop_chunk());
    }
}

void memory::pmm::frame_stack::push_chunk(memory::pmm::frame_stack_chunk * chunk)
{
    LOCK(chunk->lock);

    {
        LOCK(_lock);

        if (!_first)
        {
            _first = _last = chunk;
            _size = chunk->size;

            return;
        }
    }

    if (chunk->size == frame_stack_chunk::max)
    {
        LOCK(_first->lock);

        chunk->prev = nullptr;
        chunk->next = _first;
        _first->prev = chunk;
        _first = chunk;
    }

    else
    {
        LOCK(_last->lock);

        chunk->prev = _last;
        chunk->next = _last->next;
        _last->next = chunk;
        _last = chunk;
    }

    _size += chunk->size;
}

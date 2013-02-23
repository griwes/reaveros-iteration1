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

#include <memory/index_stack.h>
#include <screen/screen.h>

extern "C" void __lock(uint8_t *);
extern "C" void __unlock(uint8_t *);

memory::index_stack::index_stack() : _stack(nullptr), _size(0), _capacity(0), _lock(0)
{
}

memory::index_stack::index_stack(uint64_t stack_address, uint64_t bottom, uint64_t top, uint64_t max)
    : _global(nullptr), _stack((uint64_t *)stack_address), _size(0), _capacity(0), _top(top), _max(max != 0 ? max : top), _lock(0)
{
    for (uint64_t i = bottom; i < top; ++i)
    {
        push(i);
    }
}

memory::index_stack::index_stack(uint64_t stack_address, memory::index_stack * global) : _global(global),
    _stack((uint64_t *)stack_address), _size(0), _capacity(0), _lock(0)
{
}

uint64_t memory::index_stack::pop()
{
    __lock(&_lock);
    auto guard = make_scope_guard([&](){ __unlock(&_lock); });

    if (!_size)
    {
        if (_global)
        {
            for (uint64_t i = 0; i < (_global->size() > 512 ? 512 : _global->size()); ++i)
            {
                push(_global->pop());
            }

            if (_global->size() == 0)
            {
                _global->_add();
            }
        }

        else if (_top != _max)
        {
            _add();
        }

        else
        {
            screen::transaction();
            screen::print("In stack at ", _stack, "");
            PANIC("Stack exhausted");
        }

        return pop();
    }

    uint64_t ret = _stack[--_size];

    if (_capacity - _size > 512)
    {
        _shrink();
    }

    return ret;
}

void memory::index_stack::push(uint64_t idx)
{
    __lock(&_lock);
    auto guard = make_scope_guard([&](){ __unlock(&_lock); });

    if (_global && _size == core_index_stack_size)
    {
        for (uint64_t i = 0; i < 4097; ++i)
        {
            _global->push(_stack[--_size]);
        }

        for (uint64_t i = 0; i < 8; ++i)
        {
            _shrink();
        }
    }

    if (_size == _capacity)
    {
        _expand();
    }

    _stack[_size++] = idx;

    if (_global)
    {
        while (_size > _global->size())
        {
            _global->push(_stack[--_size]);
        }

        while (_capacity - _size > 512)
        {
            _shrink();
        }
    }
}

void memory::index_stack::_expand()
{
    memory::vm::map((uint64_t)_stack + _capacity * 8);

    _capacity += 512;
}

void memory::index_stack::_shrink()
{
    _capacity -= 512;

    memory::vm::unmap((uint64_t)_stack + _capacity * 8);
}

void memory::index_stack::_add()
{
    _expand();

    for (uint64_t i = _top; i < (_top + 512 < _max ? _top + 512 : _max); ++i)
    {
        _stack[_size++] = i;
    }
}

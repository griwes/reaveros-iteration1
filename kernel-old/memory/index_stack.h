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

#pragma once

namespace memory
{
    class index_stack
    {
    public:
        index_stack();
        index_stack(uint64_t, uint64_t, uint64_t, uint64_t = 0);
        index_stack(uint64_t, index_stack *);

        uint64_t pop();
        void push(uint64_t);

        uint64_t size()
        {
            return _size;
        }

    private:
        void _expand();
        void _shrink();
        void _add();

        index_stack * _global;

        uint64_t * _stack;
        uint64_t _size;
        uint64_t _capacity;
        uint64_t _top;
        uint64_t _max;
        uint8_t _lock;
    };
}

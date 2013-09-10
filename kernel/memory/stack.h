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

#include <utils/spinlock.h>

namespace memory
{
    struct map_entry;

    namespace pmm
    {
        struct frame_stack_chunk
        {
            static constexpr uint64_t max = 509 - sizeof(utils::recursive_spinlock) / 8;

            frame_stack_chunk * prev;
            frame_stack_chunk * next;
            utils::recursive_spinlock lock;
            uint64_t size;
            uint64_t stack[max];
        };

        static_assert(sizeof(frame_stack_chunk) == 4096, "wrong size of frame stack chunk");

        class frame_stack
        {
        public:
            frame_stack();
            frame_stack(map_entry *, uint64_t);

            uint64_t pop();
            void push(uint64_t);

            frame_stack_chunk * pop_chunk();
            void push_chunk(frame_stack_chunk *);

            uint64_t size() const
            {
                return _size;
            }

            void set_balancing(uint64_t balance)
            {
                _balance = balance * frame_stack_chunk::max;
            }

        private:
            frame_stack_chunk * _first;
            frame_stack_chunk * _last;
            std::atomic<uint64_t> _size;
            frame_stack * _global;
            utils::spinlock _lock;
            uint64_t _balance;
        };
    }
}

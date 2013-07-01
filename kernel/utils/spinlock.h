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

#pragma once

#include <atomic>

#include <processor/processor.h>

namespace utils
{
    class spinlock
    {
    public:
        void lock()
        {
            while (_lock.exchange(true))
            {
                asm volatile ("pause");
            }
        }

        void unlock()
        {
            if (!_lock)
            {
                PANIC("tried to unlock unlocked spinlock!");
            }

            _lock = false;
        }

    private:
        std::atomic<bool> _lock;
    };

    class recursive_spinlock
    {
    public:
        void lock()
        {
            uint64_t cpu = processor::initial_id();

            while (_owner != cpu)
            {
                LOCK(_inner_lock);

                if (_count != 0)
                {
                    while (_count != 0)
                    {
                        asm volatile ("pause");
                    }

                    continue;
                }

                _owner = cpu;
                ++_count;
            }
        }

        void unlock()
        {
            uint64_t cpu = processor::initial_id();

            if (_count != 0 && _owner != cpu)
            {
                PANIC("invalid unlock on recursive spinlock!");
            }

            --_count;
        }

    private:
        spinlock _inner_lock;
        std::atomic<uint64_t> _count;
        std::atomic<uint64_t> _owner;
    };
}

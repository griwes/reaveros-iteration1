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

namespace utils
{
    template<typename T>
    class unique_lock
    {
    public:
        unique_lock(T & lock) : _lock{ lock }
        {
            _lock.lock();
        }

        ~unique_lock()
        {
            _lock.unlock();
        }

        unique_lock(const unique_lock<T> &) = delete;
        unique_lock(unique_lock<T> &&) = default;
        unique_lock<T> & operator=(const unique_lock<T> &) = delete;
        unique_lock<T> & operator=(unique_lock<T> &&) = delete;

    private:
        T & _lock;
    };

    template<typename T>
    unique_lock<T> make_unique_lock(T & lock)
    {
        return { lock };
    }

    class bit_lock
    {
    public:
        bit_lock(uint64_t * address, uint8_t bit) : _address{ address }, _bit{ bit }
        {
            while (__sync_fetch_and_or(address, 1 << (_bit - 1)) & (1 << (_bit - 1)))
            {
                asm volatile ("pause" ::: "memory");
            }
        }

        ~bit_lock()
        {
            __sync_fetch_and_and(_address, ~static_cast<uint64_t>(1 << (_bit - 1)));
        }

        bit_lock(const bit_lock &) = delete;
        bit_lock(bit_lock &&) = default;
        bit_lock & operator=(const bit_lock &) = delete;
        bit_lock & operator=(bit_lock &&) = delete;

    private:
        uint64_t * _address;
        uint8_t _bit;
    };

    class interrupt_lock
    {
    public:
        interrupt_lock()
        {
            uint64_t flags = 0;
            asm volatile ("pushfq; pop %%rax" : "=a"(flags) :: "rax");

            _enable = flags & (1 << 9);

            asm volatile ("cli");
        }

        ~interrupt_lock()
        {
            if (_enable)
            {
                asm volatile ("sti");
            }
        }

    private:
        bool _enable;
    };

    inline interrupt_lock cli()
    {
        return {};
    }
}

#define CONCATEX(x, y) x ## y
#define CONCAT(x, y) CONCATEX(x, y)
#define LOCK(x) auto CONCAT(_, __LINE__) = utils::make_unique_lock(x);
#define INTL() auto CONCAT(_, __LINE__) = utils::cli();

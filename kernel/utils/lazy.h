/**
 * Reaver Project OS, Rose License
 *
 * Copyright © 2014 Michał "Griwes" Dominiak
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
 **/

#pragma once

#include <type_traits>
#include <atomic>
#include <utility>

#include <utils/spinlock.h>
#include <utils/locks.h>

namespace utils
{
    template<typename T>
    class lazy
    {
    public:
        lazy() = default;

        template<typename... Args>
        void initialize(Args &&... args)
        {
            LOCK(_spinlock);
            ASSERT(!_initialized);
            new (static_cast<virt_addr_t>(&_storage)) T{ std::forward<Args>(args)... };
            _initialized = true;
        }

        T * operator->()
        {
            ASSERT(_initialized);
            return reinterpret_cast<T *>(&_storage);
        }

        const T * operator->() const
        {
            ASSERT(_initialized);
            return reinterpret_cast<const T *>(&_storage);
        }

        T & operator*()
        {
            return *operator->();
        }

        const T & operator*() const
        {
            return *operator->();
        }

    private:
        std::atomic<bool> _initialized{ false };
        spinlock _spinlock;
        std::aligned_storage_t<sizeof(T), alignof(T)> _storage;
    };
}

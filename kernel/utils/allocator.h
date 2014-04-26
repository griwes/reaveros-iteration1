/**
 * Reaver Project OS, Rose License
 *
 * Copyright © 2013 Michał "Griwes" Dominiak
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

#include <utils/spinlock.h>

namespace utils
{
    template<typename T>
    uint64_t allocate_id()
    {
        static std::atomic<uint64_t> id{ 1 };
        return id++;
    }

    template<typename>
    class chained;

    template<typename T>
    class allocator
    {
    public:
        static_assert(std::is_base_of<chained<T>, T>::value, "template parameter of utils::allocator must be chained.");

        T * allocate()
        {
            INTL();
            LOCK(_lock);

            T * ret;

            if (unlikely(!_free_count))
            {
                _free = allocate_chained<T>();
                _free_count = 4096 > sizeof(T) ? 4096 / sizeof(T) : 1;
            }

            ret = _free;
            _free = _free->next;

            return ret;
        }

        void free(T * ptr)
        {
            INTL();
            LOCK(_lock);

            ++_free_count;

            if (likely(_free_count))
            {
                ptr->next = _free->next;
                ptr->prev = _free;

                if (likely(_free->next))
                {
                    _free->next->prev = ptr;
                }

                _free->next = ptr;
            }

            else
            {
                _free = ptr;
            }
        }

    private:
        uint64_t _free_count = 0;
        T * _free = nullptr;
        utils::spinlock _lock;
    };

    namespace _detail
    {
        template<typename T>
        class _default_allocator
        {
        public:
            static T * allocate()
            {
                return _a.allocate();
            }

            static void free(T * ptr)
            {
                return _a.free(ptr);
            }

        private:
            static allocator<T> _a;
        };

        template<typename T>
        allocator<T> _default_allocator<T>::_a;
    }

    template<typename T>
    class chained
    {
    public:
        T * next = nullptr;
        T * prev = nullptr;

        void * operator new(uint64_t) noexcept
        {
            return _detail::_default_allocator<T>::allocate();
        }

        void operator delete(void * ptr) noexcept
        {
            return _detail::_default_allocator<T>::free((T *)ptr);
        }

        void * operator new(uint64_t, allocator<T> & a) noexcept
        {
            return a.allocate();
        }

        void operator delete(void * ptr, allocator<T> & a) noexcept
        {
            a.free((T *)ptr);
        }

        void * operator new[](uint64_t) = delete;
        void operator delete[](void *) = delete;
    };
}

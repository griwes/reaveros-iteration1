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

#include <utils/allocator.h>
#include <screen/screen.h>

namespace utils
{
    template<typename T, typename Comparator>
    class priority_list
    {
    public:
        priority_list() : _comp{}, _first{}, _size{}
        {
        }

        ~priority_list()
        {
        }

        template<typename Initializer>
        void insert(const Initializer & f)
        {
            INTL();
            LOCK(_lock);

            ++_size;

            T * n = new (_allocator) T{};
            f(*n);

            if (unlikely(!_first))
            {
                _first = n;
                _last = n;
            }

            else if (unlikely(_comp(*n, *_first)))
            {
                n->next = _first;
                _first = n;
            }

            else if (unlikely(_comp(*_last, *n)))
            {
                _last->next = n;
                n->prev = _last;
                _last = n;
            }

            else
            {
                T * current = _first;

                while (_comp(*n, *current))
                {
                    current = current->next;
                }

                n->prev = current;
                n->next = current->next;
                current->next->prev = n;
                current->next = n;
            }
        }

        void insert(const T & value)
        {
            insert([&](T & ref){ ref = value; });
        }

        void insert(T && value)
        {
            insert([&](T & ref){ ref = std::move(value); });
        }

        template<typename EqualCompare, typename Update>
        void update(const EqualCompare & c, const Update & u)
        {
            bool success = true;
            T v = remove(c, success);

            if (!success)
            {
                return;
            }

            u(v);
            insert(v);
        }

        template<typename EqualCompare>
        void update(const EqualCompare & c, const T & val)
        {
            update(c, [&](T & ref){ ref = val; });
        }

        template<typename EqualCompare>
        void update(const EqualCompare & c, T && val)
        {
            update(c, [&](T & ref){ ref = std::move(val); });
        }

        void update(const T & ref, const T & val)
        {
            update([&](const T & r){ return r == ref; }, [&](T & ref){ ref = val; });
        }

        void update(const T & ref, const T && val)
        {
            update([&](const T & r){ return r == ref; }, [&](T & ref){ ref = std::move(val); });
        }

        template<typename EqualCompare>
        T remove(const EqualCompare & c, bool & success)
        {
            INTL();
            LOCK(_lock);

            if (unlikely(!_size))
            {
                success = false;
                return {};
            }

            --_size;

            T * current = _first;

            while (current && !c(*current))
            {
                current = current->next;
            }

            if (unlikely(!current))
            {
                success = false;
                return {};
            }

            if (current->prev)
            {
                current->prev->next = current->next;
            }

            else
            {
                _first = current->next;
            }

            if (current->next)
            {
                current->next->prev = current->prev;
            }

            else
            {
                _last = current->prev;
            }

            current->next = nullptr;
            current->prev = nullptr;

            T ret = std::move(*current);
            current->~T();
            _allocator.free(current);

            return ret;
        }

        template<typename EqualCompare>
        T remove(const EqualCompare & c)
        {
            bool success = true;
            auto ret = remove(c, success);

            if (!success)
            {
                PANIC("Tried to remove non-existing element from priority list.");
            }

            return ret;
        }

        T remove(const T & r)
        {
            return remove([&](const T & ref){ return r == ref; });
        }

        template<typename EqualCompare>
        const T * find(const EqualCompare & c)
        {
            if (unlikely(!_size))
            {
                return nullptr;
            }

            if (unlikely(c(*_first)))
            {
                return _first;
            }

            if (unlikely(c(*_last)))
            {
                return _last;
            }

            if (unlikely(_size <= 1))
            {
                return nullptr;
            }

            T * current = _first->next;

            while (current && !c(*current))
            {
                current = current->next;
            }

            return current;
        }

        const T * find(const T & r)
        {
            return find([&](const T & ref){ return r == ref; });
        }

        const T * top()
        {
            return _first;
        }

        T pop()
        {
            INTL();
            LOCK(_lock);

            if (unlikely(!_size))
            {
                PANIC("Tried to pop from empty priority list.");
            }

            --_size;

            T * first = _first;
            _first = _first->next;

            if (_size)
            {
                _first->prev = nullptr;
            }

            if (!_size)
            {
                _last = nullptr;
            }

            first->prev = nullptr;
            first->next = nullptr;

            T ret = std::move(*first);
            first->~T();
            _allocator.free(first);

            return ret;
        }

        uint64_t size() const
        {
            return _size;
        }

    private:
        Comparator _comp;
        T * _first;
        T * _last;

        spinlock _lock;
        allocator<T> _allocator;

        uint64_t _size;
    };
}

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

#include <type_traits>

#include <utils/priority_list.h>

namespace utils
{
    namespace _detail
    {
        template<class T>
        struct _vfq_element : public utils::chained<_vfq_element<T>>
        {
            uint64_t vfq_priority = 0;
            uint64_t vfq_internal_data = 0;
            T value{};
        };

        template<typename T>
        struct _vfq_comparator
        {
            bool operator()(const _vfq_element<T> & lhs, const _vfq_element<T> & rhs)
            {
                return lhs.vfq_internal_data < rhs.vfq_internal_data;
            }
        };
    }

    template<typename T, uint64_t Levels>
    class variable_frequency_queue
    {
    public:
        uint64_t size()
        {
            INTL();
            LOCK(_lock);
            return _list.size();
        }

        uint64_t load()
        {
            INTL();
            LOCK(_lock);
            return _load;
        }

        T pop()
        {
            INTL();
            LOCK(_lock);

            if (!_load)
            {
                PANIC("called `pop()` on empty variable frequency queue");
            }

            if (_valid_current)
            {
                _list.insert(std::move(_current));
            }

            _current = _list.pop();
            _current.vfq_internal_data += Levels - _current.vfq_priority;
            return _current.value;
        }

        void push(uint64_t priority, T element)
        {
            INTL();
            LOCK(_lock);

            _detail::_vfq_element<T> e;
            e.vfq_priority = priority;
            e.value = std::move(element);

            if (_list.size())
            {
                e.vfq_internal_data = _list.top()->vfq_internal_data;
            }

            else
            {
                e.vfq_internal_data = 0;
            }

            _list.insert(e);
            _load += priority;
        }

        void remove(T elem)
        {
            INTL();
            LOCK(_lock);

            bool success = false;
            _list.remove([&](const _detail::_vfq_element<T> & rhs){ return rhs.value == elem; }, success);

            if (!success && _current.value == elem)
            {
                _current = {};
                _valid_current = false;
            }
        }

    private:
        utils::spinlock _lock;
        utils::priority_list<_detail::_vfq_element<T>, _detail::_vfq_comparator<T>> _list;
        _detail::_vfq_element<T> _current;
        bool _valid_current = false;
        uint64_t _load = 0;
    };
}

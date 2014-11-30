/**
 * Reaver Project OS, Rose License
 *
 * Copyright © 2013-2014 Michał "Griwes" Dominiak
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

#include <utils/allocator.h>
#include <scheduler/scheduler.h>
#include <utils/hash_map.h>

namespace scheduler
{
    template<typename T>
    class manager
    {
    public:
        static_assert(std::is_same<T, process>::value || std::is_same<T, thread>::value, "manager only for thread and process");

        manager()
        {
        }

        T * allocate()
        {
            T * ptr = new T{};
            ptr->id = utils::allocate_id<T>();
            _map.insert(ptr->id, ptr);
            return ptr;
        }

        void free(uint64_t id)
        {
            if (!_map.contains())
            {
                return;
            }

            T * ptr = _map[id];
            _map.remove(id);
            delete ptr;
        }

        T * operator[](uint64_t id)
        {
            if (!_map.contains(id))
            {
                return nullptr;
            }

            return _map[id];
        }

    private:
        utils::hash_map<uint64_t, T *> _map;
    };
}

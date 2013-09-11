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

#include <utils/allocator.h>
#include <screen/screen.h>

namespace scheduler
{
    struct manager_chunk
    {
        manager_chunk(uint64_t i = 0) : prev{ nullptr }, next{ nullptr }, next_free{ nullptr }, free_index{ 0 }
        {
            for (uint64_t j = 0; j < max - 1; ++j)
            {
                addresses[j] = i + j + 1;
            }

            addresses[max - 1] = ~0ull;
        }

        static constexpr uint64_t max = 508 + 15 * 512;

        manager_chunk * prev;
        manager_chunk * next;
        manager_chunk * next_free;

        uint64_t free_index;
        uint64_t addresses[max];
    };

    template<typename T>
    class manager
    {
    public:
        static_assert(std::is_same<T, process>::value || std::is_same<T, thread>::value, "manager only for thread and process");

        manager(decltype(nullptr))
        {
        }

        manager()
        {
            _first = allocate_chained<manager_chunk>();
            _last = _first;
            _first_free = _first;
            _max_id = manager_chunk::max;

            screen::debug("\n", _first_free->addresses[_first_free->free_index % manager_chunk::max]);
        }

        T * allocate()
        {
            LOCK(_lock);

            if (!_first_free)
            {
                _last->next = allocate_chained<manager_chunk>(_max_id);
                _last->next->prev = _last->next;
                _first_free = _last = _last->next;

                _max_id += manager_chunk::max;

                screen::debug("\n", _first_free->addresses[_first_free->free_index % manager_chunk::max]);
            }

            T * ret = new T{};
            ret->id = _first_free->free_index;
            _first_free->free_index = _first_free->addresses[ret->id % manager_chunk::max];
            _first_free->addresses[ret->id % manager_chunk::max] = (uint64_t)ret;

            if (_first_free->free_index == manager_chunk::max)
            {
                _first_free = _first_free->next;
            }

            return ret;
        }

        void free(uint64_t id)
        {
            LOCK(_lock);

            auto chunk = _get_chunk(id);

            if (!chunk || chunk->addresses[id % manager_chunk::max] < 0xFFFF800000000000 || chunk->addresses[
                id % manager_chunk::max] == ~0ull)
            {
                PANICEX("tried to remove an element with invalid ID!", [&]
                {
                    screen::print("ID: ", id);
                    screen::print("\nManager address: ", this);
                });
            }

            LOCK(((T *)chunk->addresses[id % manager_chunk::max])->lock);
            delete (T *)chunk->addresses[id % manager_chunk::max];

            auto free = chunk->free_index;
            chunk->addresses[id % manager_chunk::max] = free;
            chunk->free_index = id;

            if (free == ~0ull)
            {
                chunk->next_free = _first_free;
                _first_free = chunk;
            }
        }

        T * operator[](uint64_t id)
        {
            LOCK(_lock);

            auto chunk = _get_chunk(id);

            if (chunk && chunk->addresses[id % manager_chunk::max] >= 0xFFFF800000000000 && chunk->addresses[
                id % manager_chunk::max] != ~0ull)
            {
                return (T *)chunk->addresses[id % manager_chunk::max];
            }

            return nullptr;
        }

    private:
        manager_chunk * _get_chunk(uint64_t id)
        {
            if (id >= _max_id)
            {
                return nullptr;
            }

            auto chunk = _first;

            for (uint64_t i = 0; i < id / manager_chunk::max && chunk; ++i)
            {
                chunk = chunk->next;
            }

            return chunk;
        }

        utils::spinlock _lock;

        manager_chunk * _first;
        manager_chunk * _last;
        manager_chunk * _first_free;

        uint64_t _max_id;
    };
}

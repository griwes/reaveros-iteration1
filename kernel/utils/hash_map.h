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

#include <utils/allocator.h>
#include <utils/locks.h>
#include <memory/vm.h>
#include <memory/memory.h>

#include <type_traits>

namespace utils
{
    template<typename T>
    struct dumb_int_hash
    {
        uint64_t operator()(const T & t) const
        {
            return t;
        }
    };

    template<typename Key, typename Value, typename HashType = dumb_int_hash<Key>, uint64_t HashTableSize = 8192>
    class hash_map
    {
        struct _element : chained<_element>
        {
            Key key;
            Value value;
            hash_map * parent;

            _element() : key{}, value{}, parent{}
            {
            }

            _element(Key k, Value v, hash_map * p) : key{ std::move(k) }, value{ std::move(v) }, parent{ p }
            {
            }
        };

    public:
        hash_map() : _hasher{}
        {
            _allocate_table();
        }

        hash_map(const HashType & h) : _hasher{ h }
        {
            _allocate_table();
        }

        void insert(const Key & k, const Value & v)
        {
            INTL();
            LOCK(_lock);

            _insert(k, v);
        }

        Value & operator[](const Key & k)
        {
            INTL();
            LOCK(_lock);

            auto elem = _find(k);

            if (!elem)
            {
                _insert(k, Value{});
                elem = _find(k);
            }

            return elem->value;
        }

        bool contains(const Key & k)
        {
            INTL();
            LOCK(_lock);

            return _find(k);
        }

        void remove(const Key & k)
        {
            INTL();
            LOCK(_lock);

            auto elem = _find(k);

            if (elem)
            {
                if (elem->next && elem->prev)
                {
                    elem->next->prev = elem->prev;
                    elem->prev->next = elem->next;
                }

                else if (elem->prev)
                {
                    elem->prev->next = nullptr;
                }

                else
                {
                    _hash_table[_hasher(k) % HashTableSize] = elem->next;
                }
            }

            --_size;
            delete elem;
        }

        uint64_t size() const
        {
            return _size;
        }

    private:
        void _allocate_table()
        {
            auto table = memory::vm::allocate_address_range(sizeof(_element *) * HashTableSize);
            memory::vm::map(table, table + sizeof(_element *) * HashTableSize);
            _hash_table = static_cast<_element **>(table);
            memory::zero(_hash_table, HashTableSize);
        }

        void _insert(const Key & k, const Value & v)
        {
            uint64_t hash = _hasher(k) % HashTableSize;

            if (!_hash_table[hash])
            {
                ++_size;
                _hash_table[hash] = new (_allocator) _element{ k, v, this };

                return;
            }

            for (auto current = _hash_table[hash]; current; current = current->next)
            {
                if (current->key == k)
                {
                    current->value = v;
                    return;
                }
            }

            ++_size;

            auto n = new (_allocator) _element{ k, v, this };
            n->next = _hash_table[hash];

            _hash_table[hash]->prev = n;
            _hash_table[hash] = n;

            return;
        }

        _element * _find(const Key & k)
        {
            uint64_t hash = _hasher(k) % HashTableSize;

            for (auto current = _hash_table[hash]; current; current = current->next)
            {
                if (current->key == k)
                {
                    return current;
                }
            }

            return nullptr;
        }

        HashType _hasher;
        _element ** _hash_table = nullptr;
        std::atomic<uint64_t> _size{};
        allocator<_element> _allocator;
        spinlock _lock;
    };
}

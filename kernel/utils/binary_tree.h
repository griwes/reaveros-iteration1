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

namespace utils
{
    template<typename Value>
    class binary_ptr_tree
    {
    public:
        void insert(Value * val)
        {
            LOCK(_lock);

            if (!_root)
            {
                _root = new _node{ val };

                return;
            }
        }

        void remove(uint64_t key);

        Value * operator[](uint64_t key)
        {
            if (!_root)
            {
                PANIC("Tried to access an element in an empty tree.");
            }

            auto ptr = check(key);

            if (!ptr)
            {
                PANIC("Tried to access an non-existant element of a tree.");
            }

            return *ptr;
        }

        Value ** check(uint64_t key)
        {
            LOCK(_lock);

            if (!_root)
            {
                return nullptr;
            }

            auto current = _root;

            while (current->value->id != key)
            {
                if (key < current->value->id)
                {
                    current = current->left;
                }

                else if (current->value->id < key)
                {
                    current = current->right;
                }

                if (!current)
                {
                    return nullptr;
                }
            }

            return &current->value;
        }

    private:
        class _node : public utils::chained<_node>
        {
            _node(Value * val) : parent{}, left{}, right{}, value{ val }
            {
            }

            _node * parent;
            _node * left;
            _node * right;
            Value * value;
        };

        _node * _root = nullptr;
        utils::spinlock _lock;
    };
}

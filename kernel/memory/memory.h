/**
 * Reaver Project OS, Rose License
 *
 * Copyright (C) 2011-2012 Reaver Project Team:
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

#include <memory/map.h>
#include <screen/mode.h>

namespace memory
{
    template<typename T>
    void zero(T * ptr, uint64_t count = 1)
    {
        zero((uint8_t *)ptr, sizeof(T) * count);
    }

    inline void zero(uint8_t * ptr, uint64_t count = 1)
    {
        uint64_t * ptrl = (uint64_t *)ptr;

        for (uint64_t i = 0; i < count / 8; ++i)
        {
            *ptrl++ = {};
        }

        for (uint64_t i = 0; i < count % 8; ++i)
        {
            *(ptr + count - count % 8 + i) = {};
        }
    }

    template<typename T>
    void copy(T * src, T * dest, uint64_t count = 1)
    {
        for (uint64_t i = 0; i < count; ++i)
        {
            *dest++ = *src++;
        }
//        copy((uint8_t *)src, (uint8_t *)dest, sizeof(T) * count);
    }

    inline void copy(uint8_t * src, uint8_t * dest, uint64_t count = 1)
    {
        uint64_t * srcl = (uint64_t *)src;
        uint64_t * destl = (uint64_t *)dest;

        for (uint64_t i = 0; i < count / 8; ++i)
        {
            *destl++ = *srcl++;
        }

        for (uint64_t i = 0; i < count % 8; ++i)
        {
            *(dest + count - count % 8 + i) = *(src + count - count % 8 + i);
        }
    }

    void copy_bootloader_data(screen::mode *&, map_entry *&, uint64_t);
    void initialize_paging();
    void drop_bootloader_mapping(bool = true);
}

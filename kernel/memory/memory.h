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

#include <screen/mode.h>
#include <memory/map.h>

namespace memory
{
    template<typename T>
    void zero(T * src, uint64_t count = 1)
    {
        while (count--)
        {
            *src++ = {};
        }
    }

    template<>
    inline void zero<uint8_t>(uint8_t * src, uint64_t count)
    {
        auto srcl = reinterpret_cast<uint64_t *>(src);

        for (uint64_t i = 0; i < count / 8; ++i)
        {
            *srcl++ = {};
        }

        for (uint64_t i = 0; i < count % 8; ++i)
        {
            *(srcl + count - count % 8 + i) = {};
        }
    }

    template<typename T>
    void copy(const T * src, T * dest, uint64_t count = 1)
    {
        while (count--)
        {
            *dest++ = *src++;
        }
    }

    template<>
    inline void copy<uint8_t>(const uint8_t * src, uint8_t * dest, uint64_t count)
    {
        auto srcl = reinterpret_cast<const uint64_t *>(src);
        auto destl = reinterpret_cast<uint64_t *>(dest);

        for (uint64_t i = 0; i < count / 8; ++i)
        {
            *destl++ = *srcl++;
        }

        for (uint64_t i = 0; i < count % 8; ++i)
        {
            *(dest + count - count % 8 + i) = *(src + count - count % 8 + i);
        }
    }

    template<typename T>
    void set(T * dest, const T & val, uint64_t count = 1)
    {
        for (uint64_t i = 0; i < count; ++i)
        {
            *dest++ = val;
        }
    }

    void copy_bootloader_data(screen::mode *&, map_entry *&, uint64_t);
    void drop_bootloader_mapping(bool = true);
}
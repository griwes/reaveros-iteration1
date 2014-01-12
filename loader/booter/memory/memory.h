/**
 * Reaver Project OS, Rose License
 *
 * Copyright © 2011-2013 Michał "Griwes" Dominiak
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

#include <memory/memmap.h>
#include <processor/processor.h>
#include <memory/manager.h>
#include <memory/x64paging.h>

namespace memory
{
    extern manager::placement_allocator default_allocator;
    extern x64::pml4 * vas;

    void initialize(uint32_t, map_entry *, uint32_t);
    void prepare_long_mode();

    uint64_t install_kernel(uint32_t, uint32_t);
    void install_kernel_stack(uint64_t &);
    void install_initrd(uint64_t, uint32_t, uint32_t);

    template<typename T>
    void zero(T * ptr, uint32_t size = 1)
    {
        while (size-- > 0)
        {
            *ptr++ = {};
        }
    }

    template<typename T>
    void copy(T * src, T * dest, uint32_t size = 1)
    {
        while (size-- > 0)
        {
            *dest++ = *src++;
        }
    }

    inline void zero(uint8_t * ptr, uint32_t size)
    {
        uint8_t * lastbytes = ptr + size - size % 4;

        zero((uint32_t *)ptr, size / 4);

        size %= 4;

        while (size-- > 0)
        {
            *lastbytes++ = {};
        }
    }

    inline void copy(uint8_t * src, uint8_t * dest, uint32_t size)
    {
        uint8_t * lastbytessrc = src + size - size % 4;
        uint8_t * lastbytesdest = dest + size - size % 4;

        copy((uint32_t *)src, (uint32_t *)dest, size / 4);

        size %= 4;

        while (size-- > 0)
        {
            *lastbytesdest++ = *lastbytessrc++;
        }
    }
}

/**
 * ReaverOS
 * kernel/types.h
 * Typedefs for int types.
 */

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
 * including commercial applications, adn to alter it and redistribute it
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

#ifndef _rose_kernel_types_h_
#define _rose_kernel_types_h_

#include <cstdint>
#include <cstddef>

typedef size_t size;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef intptr_t intptr;
typedef uintptr_t uintptr;

template<typename T>
inline T abs(T i)
{
    if (i < 0)
    {
        return -i;
    }

    return i;
}

#define PANIC(X) _panic(X, __FILE__, __LINE__, __PRETTY_FUNCTION__)

void _panic(const char *, const char *, uint64, const char *);

#define dbg __asm("xchg %bx, %bx")

#endif
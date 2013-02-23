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

#include <cstdint>
#include <cstddef>

#define dbg asm ("xchg %bx, %bx")

#define PANIC(X) _panic(X, __FILE__, __LINE__, __PRETTY_FUNCTION__)

void _panic(const char *, const char *, uint64_t, const char *);

void * operator new(uint32_t, void *);

inline void inb(uint16_t, uint8_t);
inline void inw(uint16_t, uint16_t);

inline void outb(uint16_t port, uint8_t value)
{
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

inline void outw(uint16_t, uint16_t);

inline void rdmsr(uint32_t msr, uint32_t & low, uint32_t & high)
{
    asm volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
}

inline void wrmsr(uint32_t msr, uint32_t low, uint32_t high)
{
    asm volatile ("wrmsr" :: "a"(low), "d"(high), "c"(msr));
}

inline void invlpg(uint32_t address)
{
    asm volatile ("invlpg (%0)" :: "r"(address) : "memory");
}

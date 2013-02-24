/**
 * Reaver Project OS, Rose License
 *
 * Copyright (C) 2011-2013 Reaver Project Team:
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

#define dbg asm volatile ("xchg %bx, %bx")
#define cli asm volatile ("cli")
#define sti asm volatile ("sti")

#include <cstdint>
#include <cstddef>

#define PANIC(X) ::_panic(X, __FILE__, __LINE__, __PRETTY_FUNCTION__)
#define DUMP(X) _dump_registers(X);

void _panic(const char *, const char *, uint64_t, const char *);
template<typename T>
void _dump_registers(const T &);

inline void * operator new (uint64_t, void * addr)
{
    return addr;
}

inline void outb(uint16_t port, uint8_t value)
{
    asm volatile ("outb %1, %0" :: "dN" (port), "a" (value));
}

inline
uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}
inline void rdmsr(uint32_t msr, uint32_t & low, uint32_t & high)
{
    asm volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
}

inline void wrmsr(uint32_t msr, uint32_t low, uint32_t high)
{
    asm volatile ("wrmsr" :: "a"(low), "d"(high), "c"(msr));
}

template<typename T>
struct scope_guard
{
    ~scope_guard()
    {
        callback();
    }

    T callback;
};

template<typename T>
scope_guard<T> make_scope_guard(T callback)
{
    return scope_guard<T>{callback};
}

namespace memory
{
    constexpr uint64_t stack_size = 64 * 1024 * 1024;
    constexpr uint64_t core_index_stack_size = 8 * 1024 * 1024;
    constexpr uint64_t max_memory_supported = 64ull * 1024 * 1024 * 1024;
}

namespace processor
{
    constexpr uint64_t max_cores = 512;
    constexpr uint64_t max_ioapics = 16;
}

namespace scheduler
{
    constexpr uint64_t max_processes = 64 * 1024 * 1024;
    constexpr uint64_t max_threads = 1024 * 1024 * 1024;
}

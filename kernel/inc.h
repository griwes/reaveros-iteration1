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
#define CLI asm volatile ("cli")
#define STI asm volatile ("sti")

#include <cstdint>
#include <cstddef>

#define PANIC(X) ::_panic(X, __FILE__, __LINE__, __PRETTY_FUNCTION__)
#define PANICEX(X, Y) ::_panic(X, Y, __FILE__, __LINE__, __PRETTY_FUNCTION__)
#define DUMP(X) _dump_registers(X);

void _panic(const char *, const char *, uint64_t, const char *, bool = false);

template<typename F>
void _panic(const char * message, const F & f, const char * file, uint64_t line, const char * func)
{
    _panic(message, file, line, func, true);
    f();

    while (true)
    {
        asm volatile ("cli; hlt");
    }

//    debugger::start();
}

inline void * operator new(uint64_t, void * addr)
{
    return addr;
}

inline void outb(uint16_t port, uint8_t value)
{
    asm volatile ("outb %1, %0" :: "dN" (port), "a" (value));
}

inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

inline void rdmsr(uint32_t msr, uint32_t & low, uint32_t & high)
{
    asm volatile ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr));
}

inline uint64_t rdmsr(uint32_t msr)
{
    uint32_t low, high;
    rdmsr(msr, low, high);

    return ((uint64_t)high << 32) | low;
}

inline void wrmsr(uint32_t msr, uint32_t low, uint32_t high)
{
    asm volatile ("wrmsr" :: "a"(low), "d"(high), "c"(msr));
}

inline void wrmsr(uint32_t msr, uint64_t val)
{
    wrmsr(msr, val & 0xFFFFFFFF, val >> 32);
}

struct pci_vendor_t
{
    uint16_t vendor;
} __attribute__((packed));

#include <memory/vm.h>

template<typename T>
T * allocate_chained(uint64_t physical = 0)
{
    static_assert(4096 % sizeof(T) == 0 || sizeof(T) % 4096 == 0, "wrong chained type requested");

    if (sizeof(T) >= 4096)
    {
        auto address = memory::vm::allocate_address_range(sizeof(T));

        if (physical)
        {
            memory::vm::map_multiple(address, address + sizeof(T), physical);
        }

        else
        {
            memory::vm::map_multiple(address, address + sizeof(T));
        }

        return new ((void *)address) T{};
    }

    T * address = (T *)memory::vm::allocate_address_range(4096);
    memory::vm::map((uint64_t)address, physical ? physical : memory::pmm::pop());

    for (uint64_t i = 0; i < 4096 / sizeof(T); ++i)
    {
        new (address + i) T{};
        address[i].prev = (i != 0 ? address + i - 1 : nullptr);
        address[i].next = (i != 4096 / sizeof(T) - 1 ? address + i + 1 : nullptr);
    }

    return address;
}

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

#define dbg asm volatile ("xchg %bx, %bx")
#define CLI asm volatile ("cli" ::: "memory")
#define STI asm volatile ("sti" ::: "memory")
#define HLT asm volatile ("hlt" ::: "memory")
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#include <cstdint>
#include <cstddef>

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::int8_t;
using std::int16_t;
using std::int32_t;
using std::int64_t;

#include <utils/types.h>

#define PANIC(X) ::_panic(X, __FILE__, __LINE__, __PRETTY_FUNCTION__); __builtin_unreachable()
#define PANICEX(X, Y) ::_panic(X, Y, __FILE__, __LINE__, __PRETTY_FUNCTION__); __builtin_unreachable()
#define DUMP(X) _dump_registers(X);
#define TODO PANIC("TODO: implement me!")
#define TODOEX(X) PANIC("TODO: implement me: " X)
#define NEVER PANIC("This function was not supposed to be ever called; file a bug report.")

#include <atomic>

extern std::atomic<bool> panicing;

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

inline void * operator new(uint64_t, virt_addr_t addr)
{
    return reinterpret_cast<void *>(static_cast<uint64_t>(addr));
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

#include <memory/vm.h>
#include <utils/aligner.h>

constexpr bool is_power_of_two(uint64_t size)
{
    return (size == 2 ? true : size % 2 == 0 && is_power_of_two(size / 2));
}

template<typename T, typename... Args>
typename utils::aligner<T>::type * allocate_chained(phys_addr_t physical = {}, const Args &... args = {})
{
    if (sizeof(typename utils::aligner<T>::type) >= 4096)
    {
        auto address = memory::vm::allocate_address_range(sizeof(T));

        if (physical)
        {
            memory::vm::map(address, address + sizeof(T), physical);
        }

        else
        {
            memory::vm::map(address, address + sizeof(T));
        }

        return ::new (address) typename utils::aligner<T>::type{ args... };
    }

    auto address = memory::vm::allocate_address_range(4096);
    memory::vm::map(address, physical ? physical : memory::pmm::pop());

    for (uint64_t i = 0; i < 4096 / (sizeof(typename utils::aligner<T>::type) > 4096 ? 1 : sizeof(typename utils::aligner<T>
        ::type)); ++i)
    {
        auto ptr = ::new (address + i * sizeof(typename utils::aligner<T>::type)) typename utils::aligner<T>::type{ args... };
        ptr->prev = (i != 0 ? ptr - 1 : nullptr);
        ptr->next = (i != 4096 / sizeof(T) - 1 ? ptr + 1 : nullptr);
    }

    return static_cast<typename utils::aligner<T>::type *>(address);
}

constexpr uint64_t operator "" _s(unsigned long long seconds)
{
    return seconds * 1000 * 1000 * 1000;
}

constexpr uint64_t operator "" _ms(unsigned long long milliseconds)
{
    return milliseconds * 1000 * 1000;
}

constexpr uint64_t operator "" _us(unsigned long long microseconds)
{
    return microseconds * 1000;
}

constexpr uint64_t operator "" _ns(unsigned long long nanoseconds)
{
    return nanoseconds;
}

template<typename T>
T max(T a, T b)
{
    return a > b ? a : b;
}

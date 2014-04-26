/**
* Reaver Project OS, Rose License
*
* Copyright © 2014 Michał "Griwes" Dominiak
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

#include <processor/syscalls.h>
#include <screen/screen.h>

namespace
{
    constexpr uint64_t _star = 0xC0000081;
    constexpr uint64_t _lstar = 0xC0000082;
    constexpr uint64_t _cstar = 0xC0000083;
    constexpr uint64_t _sfmask = 0xC0000084;
    constexpr uint64_t _ia32_efer = 0xC0000080;

    void _syscall_handler()
    {
        uint64_t number, data, ret;

        asm volatile (R"(
            mov %%rax, %0
            mov %%rbx, %1
            mov %%rcx, %2
        )" : "=r"(number), "=r"(data), "=r"(ret));

        screen::print("\nhello from syscall handler, id = ", number);
        screen::print("\ntext: ", (const char *)data, "\n");

        asm volatile (R"(
            sysret
        )" :: "c"(ret));
    }
}

void processor::syscalls::initialize()
{
    wrmsr(_ia32_efer, rdmsr(_ia32_efer) | 1);
    wrmsr(_lstar, (uint64_t)&_syscall_handler);
    wrmsr(_star, (8ull << 32) | (24ull << 48));
}

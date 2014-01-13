/**
 * Reaver Project OS, Rose License
 *
 * Copyright © 2013-2014 Michał "Griwes" Dominiak
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

#include <utils/allocator.h>

namespace processor
{
    struct isr_context
    {
        uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
        uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
        uint64_t number, error;
        uint64_t rip, cs, rflags, rsp, ss;
    } __attribute__((packed));
    
    extern "C" void isr_context_return(isr_context &);

    struct context : public utils::chained<context>
    {
        uint64_t rax = 0, rbx = 0, rcx = 0, rdx = 0;
        uint64_t rsi = 0, rdi = 0, rsp = 0, rbp = 0;
        uint64_t r8 = 0, r9 = 0, r10 = 0, r11 = 0;
        uint64_t r12 = 0, r13 = 0, r14 = 0, r15 = 0;
        uint64_t cs = 0x8, ss = 0x10;
        uint64_t rip = 0, rflags = 1 << 9;

        void load(isr_context &);
        void save(isr_context &);
    };
}

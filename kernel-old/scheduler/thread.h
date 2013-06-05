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

#pragma once

namespace scheduler
{
    class process;
    class sse_state;
    class mailbox;

    class thread
    {
    public:
    private:
        process * _parent;
        thread * _prev;
        thread * _next;

        uint64_t _rax, _rbx, _rcx, _rdx;
        uint64_t _rbp, _rsp, _rsi, _rdi;
        uint64_t _r8, _r9, _r10, _r11;
        uint64_t _r12, _r13, _r14, _r15;
        uint64_t _rip, _cs, _ss, _rflags;
        uint64_t _kernel_cs, _kernel_ss, _kernel_rsp;

        uint64_t _priority;
        uint64_t _time;

        sse_state * _sse_state;

        mailbox * _mailbox;

        uint64_t _dead:1;
        uint64_t _sse:1;
        uint64_t _unused:62;
        uint64_t _unused2;
    };
}

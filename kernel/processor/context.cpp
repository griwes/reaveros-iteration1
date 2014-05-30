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

#include <processor/context.h>

void processor::context::save(processor::isr_context & ctx)
{
    rax = ctx.rax;
    rbx = ctx.rbx;
    rcx = ctx.rcx;
    rdx = ctx.rdx;
    rsi = ctx.rsi;
    rdi = ctx.rdi;
    rsp = ctx.rsp;
    rbp = ctx.rbp;
    r8 = ctx.r8;
    r9 = ctx.r9;
    r10 = ctx.r10;
    r11 = ctx.r11;
    r12 = ctx.r12;
    r13 = ctx.r13;
    r14 = ctx.r14;
    r15 = ctx.r15;
    cs = ctx.cs;
    ss = ctx.ss;
    rip = ctx.rip;
    rflags = ctx.rflags;
}

void processor::context::load(processor::isr_context & ctx)
{
    ctx.rax = rax;
    ctx.rbx = rbx;
    ctx.rcx = rcx;
    ctx.rdx = rdx;
    ctx.rsi = rsi;
    ctx.rdi = rdi;
    ctx.rsp = rsp;
    ctx.rbp = rbp;
    ctx.r8 = r8;
    ctx.r9 = r9;
    ctx.r10 = r10;
    ctx.r11 = r11;
    ctx.r12 = r12;
    ctx.r13 = r13;
    ctx.r14 = r14;
    ctx.r15 = r15;
    ctx.cs = cs;
    ctx.ss = ss;
    ctx.rip = rip;
    ctx.rflags = rflags;
}

void processor::context::save(processor::syscall_context & ctx)
{
    rax = ctx.syscall_number;
    rbx = ctx.rbx;
    rcx = 0;
    rdx = 0;
    rsi = ctx.rsi;
    rdi = ctx.rdi;
    rsp = ctx.user_rsp;
    rbp = ctx.rbp;
    r8 = ctx.r8;
    r9 = ctx.r9;
    r10 = ctx.r10;
    r11 = 0;
    r12 = ctx.r12;
    r13 = ctx.r13;
    r14 = ctx.r14;
    r15 = ctx.r15;
    cs = 0x1b;
    ss = 0x23;
    rip = ctx.user_rip;
    rflags = ctx.rflags;
}

void processor::context::load(processor::syscall_context & ctx)
{
    ctx.syscall_number = rax;
    ctx.rbx = rbx;
    ctx.rsi = rsi;
    ctx.rdi = rdi;
    ctx.user_rsp = rsp;
    ctx.rbp = rbp;
    ctx.r8 = r8;
    ctx.r9 = r9;
    ctx.r10 = r10;
    ctx.r12 = r12;
    ctx.r13 = r13;
    ctx.r14 = r14;
    ctx.r15 = r15;
    ctx.user_rip = rip;
    ctx.rflags = rflags;
}

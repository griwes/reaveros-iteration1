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

#include <processor/thread.h>
#include <processor/core.h>
#include <scheduler/thread.h>
#include <processor/processor.h>

void processor::set_current_thread(scheduler::thread * thread)
{
    auto previous = processor::current_thread();
    processor::core * core = nullptr;

    if (previous)
    {
        core = previous->current_core;
        previous->current_core = nullptr;
        previous->last_core = core;
    }

    else
    {
        core = processor::get_current_core();
    }

    if (!core)
    {
        PANIC("core = nullptr");
    }

    if (thread->address_space != processor::get_asid())
    {
        screen::print((void *)thread->address_space, ", ", (void *)processor::get_asid());
        processor::set_asid(thread->address_space);
    }

    if (unlikely(thread->status == scheduler::thread_status::init))
    {
        thread->context->rsp = core->_tss.rsp0;
    }

    thread->status = scheduler::thread_status::running;
    thread->current_core = core;
    core->thread = thread;

    screen::debug("\nCurrent thread on core #", processor::id(), ": ", thread->id);

    if (thread->id > processor::get_core_count() && !processor::get_current_core()->in_interrupt_handler())
    {
        isr_context ctx{};

        asm volatile (R"(
            mov     %%rbp, %0
            mov     %%rsp, %1
            mov     %%cs, %%ax
            mov     %%rax, %2
            mov     %%ss, %%ax
            mov     %%rax, %3
            pushf
            pop     %4
            movq    $1f, %5
        )" : "=m"(ctx.rbp), "=m"(ctx.rsp), "=m"(ctx.cs), "=m"(ctx.ss), "=m"(ctx.rflags), "=m"(ctx.rip) :: "rax");

        previous->save(ctx);
        thread->load(ctx);

        dbg;

        asm volatile ("jmp isr_context_return; 1:");
    }
}

void processor::enter_userspace()
{
    asm volatile(R"(
        push    $0x23
        push    %rsp
        push    $0x200
        push    $0x1b
        push    $1f

        iretq
    1:
    )");
}

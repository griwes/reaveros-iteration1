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

#include <scheduler/scheduler.h>
#include <processor/ipi.h>
#include <processor/core.h>
#include <scheduler/process.h>
#include <scheduler/thread.h>
#include <scheduler/manager.h>
#include <processor/thread.h>
#include <processor/processor.h>
#include <utils/lazy.h>

namespace
{
    utils::lazy<scheduler::manager<scheduler::process>> _pcb_manager;
    utils::lazy<scheduler::manager<scheduler::thread>> _tcb_manager;

    int64_t _score(scheduler::thread * t, processor::core * c)
    {
        int64_t score = 1024;

        if (t->last_core == c)
        {
            score += 1024;
        }

        score -= c->scheduler().load();

        if (c == processor::current_core())
        {
            score -= 128;
        }

        if (c->thread->address_space == t->address_space)
        {
            score += 256;
        }

        return score;
    }

    bool _ready = false;
}

void scheduler::initialize()
{
    INTL();

    screen::debug("\nInitializing PCB manager");
    _pcb_manager.initialize();
    screen::debug("\nInitialized PCB manager at ", &*_pcb_manager);
    screen::debug("\nInitializing TCB manager");
    _tcb_manager.initialize();
    screen::debug("\nInitialized TCB manager at ", &*_tcb_manager);

    processor::smp::parallel_execute([](uint64_t){ ap_initialize(); });

    _ready = true;
}

void scheduler::ap_initialize()
{
    INTL();

    screen::debug("\nInitializing local thread scheduler on core #", processor::id());
    processor::current_core()->initialize_scheduler();

    thread * kernel_thread = _tcb_manager->allocate();
    kernel_thread->last_core = processor::current_core();
    kernel_thread->address_space = processor::get_asid();
    kernel_thread->status = thread_status::running;
    kernel_thread->policy = scheduling_policy::idle;
    processor::current_core()->scheduler().push(kernel_thread);
    processor::current_thread(kernel_thread);

    screen::debug("\nLocal kernel thread ID on core #", processor::id(), ": ", kernel_thread->id);
}

bool scheduler::ready()
{
    return _ready;
}

scheduler::thread * scheduler::current_thread()
{
    return processor::current_thread();
}

void scheduler::schedule(scheduler::thread * t)
{
    processor::core * best_core = processor::cores();
    int64_t best_score = _score(t, best_core);

    for (uint64_t i = 1; i < processor::core_count(); ++i)
    {
        int64_t score = _score(t, processor::cores() + i);

        if (score > best_score)
        {
            best_core = processor::cores() + i;
            best_score = score;
        }
    }

    best_core->scheduler().push(t);
}

scheduler::thread * scheduler::create_thread(void * start, uint64_t data, scheduler::process * parent)
{
    INTL();

    auto new_thread = _tcb_manager->allocate();
    new_thread->status = thread_status::init;

    if (parent)
    {
        LOCK(parent->lock);

        new_thread->parent = parent;
        new_thread->address_space = parent->address_space;

        if (!parent->main_thread)
        {
            parent->main_thread = new_thread;
        }

        else
        {
            auto current = parent->main_thread;

            while (current->next_sibling)
            {
                current = current->next_sibling;
            }

            current->next_sibling = new_thread;
        }

        new_thread->policy = parent->policy;
        new_thread->priority = parent->priority;
    }

    else
    {
        new_thread->address_space = processor::get_asid();
        new_thread->policy = scheduling_policy::normal;
        new_thread->priority = 128;
    }

    new_thread->context = new processor::context{};
    new_thread->context->cs = 0x8;
    new_thread->context->ss = 0x10;
    new_thread->context->rdi = data;
    new_thread->context->rip = (uint64_t)start;

    return new_thread;
}

void scheduler::create_process(const uint8_t * image_begin, const uint8_t * image_end, process * parent, bool start, bool service)
{
    auto new_process = _pcb_manager->allocate();
    new_process->address_space = memory::vm::clone_kernel();
    memory::vm::release_foreign();

    auto old_asid = processor::get_asid();

    processor::set_asid(new_process->address_space);
    memory::vm::map(virt_addr_t{ 0x100000 }, virt_addr_t{ 0x100000 } + (image_end - image_begin), memory::vm::user);

    memory::copy(image_begin, (uint8_t *)0x100000, image_end - image_begin);

    processor::set_asid(old_asid);
    new_process->service = service;
    schedule(create_thread((void *)&processor::enter_userspace, 0x100000, new_process));
}

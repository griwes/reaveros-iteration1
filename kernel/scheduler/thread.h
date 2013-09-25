/**
 * Reaver Project OS, Rose License
 *
 * Copyright (C) 2013 Reaver Project Team:
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

#include <utils/allocator.h>
#include <scheduler/scheduler.h>
#include <processor/context.h>

namespace processor
{
    struct context;
    struct extended_context;
}

namespace scheduler
{
    struct process;
    class local;

    enum class thread_status : uint8_t
    {
        invalid,
        init,
        running,
        ready,
        sleeping,
        waiting_ipc,
        waiting_mutex,
        waiting_semaphore,
        zombie,
        dead
    };

    struct thread : public utils::chained<thread>
    {
        void save(processor::isr_context & ctx)
        {
            if (!context)
            {
                context = new processor::context{};
            }

            context->save(ctx);
        }

        void load(processor::isr_context & ctx)
        {
            if (!context)
            {
                PANIC("tried to load non existent context!");
            }

            context->load(ctx);
        }

        utils::spinlock lock;

        uint64_t id;

        process * parent;

        thread * prev;
        thread * next;
        thread * prev_sibling;
        thread * next_sibling;

        uint64_t address_space;

        processor::context * context;
        processor::extended_context * ext_context;

        thread_status status;
        processor::core * last_core;

        scheduling_policy policy;
        uint8_t priority = 128;
    };
}

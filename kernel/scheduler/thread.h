/**
 * Reaver Project OS, Rose License
 *
 * Copyright (C) 2013-2014 Reaver Project Team:
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
#include <utils/vfq.h>

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
        ~thread()
        {
            status = thread_status::invalid;
            delete context;
        }

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
                PANICEX("tried to load non existent context!", [&]
                {
                    screen::print("Thread ID: ",  id);
                });
            }

            context->load(ctx);
        }

        utils::spinlock lock;

        uint64_t id;

        process * parent = nullptr;

        thread * prev_sibling = nullptr;
        thread * next_sibling = nullptr;

        uint64_t address_space;

        processor::context * context = nullptr;
        processor::extended_context * ext_context = nullptr;

        thread_status status = thread_status::init;
        processor::core * last_core = nullptr;
        processor::core * current_core = nullptr;

        scheduling_policy policy = scheduling_policy::normal;
        uint8_t priority = 128;
    };

    inline bool valid(thread * t)
    {
        return t && t->status != thread_status::invalid && t->status != thread_status::zombie && t->status != thread_status::dead;
    }
}

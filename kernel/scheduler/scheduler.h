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

#include <atomic>

namespace scheduler
{
    struct process;
    struct thread;

    void initialize();
    void ap_initialize();

    void schedule(thread *);
    thread * create_thread(void *, uint64_t, process * = nullptr);

    inline thread * create_thread(void * start, process * parent = nullptr)
    {
        return create_thread(start, 0, parent);
    }

    bool ready();

    thread * current_thread();

    enum class scheduling_policy : uint8_t
    {
        normal,
        top,
        background,
        idle
    };
}

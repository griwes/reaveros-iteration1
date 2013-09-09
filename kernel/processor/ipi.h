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

#include <processor/lapic.h>

namespace processor
{
    inline void ipi(uint64_t target, ipis ipi, uint8_t data = 0)
    {
        get_lapic()->ipi(target, ipi, data);
    }

    inline void broadcast(broadcasts target, ipis ipi, uint8_t data = 0)
    {
        get_lapic()->broadcast(target, ipi, data);
    }

    namespace smp
    {
        enum class policies
        {
            all,
            others,
            specific,
            // TODO:
            // all_domain,
            // others_domain,
            // per_core,
            // per_chip,
            // per_domain
        };

        void initialize_parallel();
        void parallel_execute(policies, void (*)(uint64_t), uint64_t = 0, uint64_t = 0);
        inline void parallel_execute(void (*fptr)(uint64_t), uint64_t data = 0)
        {
            parallel_execute(policies::all, fptr, data);
        }
    }
}

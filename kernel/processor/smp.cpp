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

#include <processor/smp.h>
#include <processor/current_core.h>
#include <processor/core.h>
#include <memory/memory.h>
#include <screen/screen.h>
#include <processor/processor.h>
#include <memory/pmm.h>
#include <memory/stacks.h>

extern "C" uint8_t trampoline_start[];
extern "C" uint8_t trampoline_end[];

void processor::smp::boot(core * cores, uint64_t & num_cores)
{
    uint64_t trampoline_size = (uint64_t)(trampoline_end - trampoline_start);

    trampoline_size += 4095;
    trampoline_size &= ~(uint64_t)4095;

    uint64_t boot_at_once = 0x7F000 / trampoline_size;

    // INIT IPI
    for (uint64_t i = 0; i < num_cores; ++i)
    {
        current_core::ipi(cores[i].apic_id(), current_core::ipis::init);
    }

    current_core::sleep(10000000);

    for (uint64_t booted = 0; booted < num_cores; ++booted)
    {
        for (uint64_t i = booted; i < booted + boot_at_once && i < num_cores; ++i)
        {
            memory::copy(trampoline_start, (uint8_t *)0x1000 + trampoline_size * (i - booted), trampoline_size);

            *(uint64_t volatile *)(0x1000 + trampoline_size * (i - booted) + 16) = memory::x64::clone_kernel();
            memory::x64::map(0, 1024 * 1024, 0, true);

            cores[i].started = (uint8_t *)(0x1000 + trampoline_size * (i - booted));
        }

        // SIPI
        for (uint64_t i = booted; i < booted + boot_at_once && i < num_cores; ++i)
        {
            current_core::ipi(cores[i].apic_id(), current_core::ipis::sipi, (0x1000 + trampoline_size * (i - booted)) >> 12);
        }

        current_core::sleep(500000);

        // 2nd SIPI, if not started
        for (uint64_t i = booted; i < booted + boot_at_once && i < num_cores; ++i)
        {
            if (!*(cores[i].started))
            {
                current_core::ipi(cores[i].apic_id(), current_core::ipis::sipi, (0x1000 + trampoline_size * (i - booted)) >> 12);
            }
        }

        for (uint64_t i = booted; i < booted + boot_at_once && i < num_cores; ++i)
        {
            if (*(cores[i].started))
            {
                screen::print("\nCPU #", cores[i].apic_id(), " booted.");
            }

            else
            {
                screen::print("\nCPU #", cores[i].apic_id(), " failed to boot (", cores[i].started, ").");

                for (uint64_t j = i; j < num_cores - 1; ++j)
                {
                    screen::print("\nMoving CPU #", cores[j + 1].apic_id(), " into place of CPU#", cores[j].apic_id());
                    cores[j] = cores[j + 1];
                }

                --i;
                --num_cores;
                --booted;
            }
        }
    }

    processor::current_core::stop_timer();

    memory::pmm::split_frame_stack(cores, num_cores);
    memory::stack_manager::split_stack_stack(cores, num_cores);
}

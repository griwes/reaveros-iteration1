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

#include <processor/smp.h>
#include <processor/core.h>
#include <processor/lapic.h>
#include <processor/idt.h>
#include <memory/memory.h>

extern "C" uint8_t _trampoline_start[];
extern "C" uint8_t _trampoline_end[];

namespace
{
    bool _timer = false;

    void _handler(processor::idt::isr_context, uint64_t)
    {
        _timer = true;
    }
}

void processor::smp::boot(processor::core * cores, uint64_t num_cores)
{
    uint64_t trampoline_size = (uint64_t)(_trampoline_end - _trampoline_start);

    trampoline_size += 4095;
    trampoline_size &= ~(uint64_t)4095;

    uint64_t boot_at_once = 0x7F000 / trampoline_size;

    // INIT IPI
    for (uint64_t i = 0; i < num_cores; ++i)
    {
        get_lapic()->ipi(cores[i].apic_id(), ipis::init);
    }

    time::get_high_precision_timer()->one_shot(10_ms, _handler);

    while (!_timer)
    {
        HLT;
    }

    _timer = false;

    for (uint64_t booted = 0; booted < num_cores; booted += boot_at_once)
    {
        for (uint64_t i = booted; i < booted + boot_at_once && i < num_cores; ++i)
        {
            memory::copy(_trampoline_start, (uint8_t *)0x1000 + trampoline_size * (i - booted), trampoline_size);

            uint64_t foreign = memory::vm::clone_kernel();
            *(uint64_t volatile *)(0x1000 + trampoline_size * (i - booted) + 16) = foreign;
            memory::vm::map_multiple_foreign(0, 1024 * 1024, 0);
            memory::vm::release_foreign();

            cores[i]._started = (uint8_t *)(0x1000 + trampoline_size * (i - booted));
        }

        // SIPI
        for (uint64_t i = booted; i < booted + boot_at_once && i < num_cores; ++i)
        {
            get_lapic()->ipi(cores[i].apic_id(), ipis::sipi, (0x1000 + trampoline_size * (i - booted)) >> 12);
        }

        time::get_high_precision_timer()->one_shot(500_us, _handler);

        while (!_timer)
        {
            HLT;
        }

        _timer = false;

        // 2nd SIPI, if not started
        for (uint64_t i = booted; i < booted + boot_at_once && i < num_cores; ++i)
        {
            if (!*(cores[i]._started))
            {
                get_lapic()->ipi(cores[i].apic_id(), ipis::sipi, (0x1000 + trampoline_size * (i - booted)) >> 12);
            }
        }

        time::get_high_precision_timer()->one_shot(500_us, _handler);

        while (!_timer)
        {
            HLT;
        }

        _timer = false;

        for (uint64_t i = booted; i < booted + boot_at_once && i < num_cores; ++i)
        {
            if (*(cores[i]._started))
            {
                screen::debug("\nCPU #", cores[i].apic_id(), " booted");
            }

            else
            {
                screen::debug("\nCPU #", cores[i].apic_id(), " failed to boot");

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
}

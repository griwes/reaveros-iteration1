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

#include <memory/x64paging.h>
#include <scheduler/scheduler.h>
#include <processor/thread.h>

namespace processor
{
    class ioapic;
    class interrupt_entry;
    class core;

    extern "C" memory::x64::pml4 * get_cr3();
    extern "C" void reload_cr3();
    extern "C" void set_cr3(uint64_t asid);

    inline uint64_t get_asid()
    {
        return (uint64_t)get_cr3();
    }

    inline void set_asid(uint64_t asid)
    {
        set_cr3(asid);
    }

    uint64_t id();
    extern "C" uint64_t initial_id();
    uint64_t get_lapic_base();
    uint8_t translate_isa(uint8_t irq);

    ioapic * get_ioapic(uint8_t input);
    uint8_t max_ioapic_input();
    interrupt_entry * get_sources();
    core * get_core(uint64_t apic_id);
    core * get_cores();

    core * get_current_core();

    uint64_t get_core_count();

    void initialize();
    extern "C" void ap_initialize();
    bool ready();
}

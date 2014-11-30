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

#include <processor/lapic.h>
#include <processor/xapic.h>
#include <processor/x2apic.h>
#include <screen/screen.h>
#include <processor/idt.h>
#include <processor/core.h>
#include <utils/lazy.h>

namespace
{
    utils::lazy<processor::xapic> _xapic;
    utils::lazy<processor::x2apic> _x2apic;

    processor::lapic * _lapic = nullptr;
    bool _x2apic_enabled = false;
}

extern processor::gdt::tss _tss;

void processor::lapic::initialize()
{
    if (x2apic_capable() && (remapping::enabled() || initial_id() < 256))
    {
        screen::debug("\nInitializing x2APIC...");
        _x2apic.initialize();
        _lapic = &*_x2apic;

        _x2apic_enabled = true;
    }

    else if (!x2apic_capable())
    {
        screen::debug("\nInitializing xAPIC...");
        _xapic.initialize();
        _lapic = &*_xapic;
    }

    else
    {
        PANIC("Interrupt remapping has not been enabled, but BSP APIC ID >= 256. Please file a bug report with full"
            " specification of your machine.");
    }

    wrmsr(0xc0000101, (uint64_t)processor::get_current_core());
    wrmsr(0xc0000102, (uint64_t)processor::get_current_core());

    auto rsp = _tss.rsp0;
    processor::get_current_core()->kernel_stack = rsp;
}

processor::lapic * processor::get_lapic()
{
    return _lapic;
}

bool processor::x2apic_enabled()
{
    return _x2apic_enabled;
}

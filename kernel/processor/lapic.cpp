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

namespace
{
    processor::xapic _xapic;
    processor::x2apic _x2apic;

    processor::lapic * _lapic = nullptr;
    bool _x2apic_enabled = false;
}

void processor::lapic::initialize()
{
    if (x2apic_capable() && (remapping::enabled() || initial_id() < 256))
    {
        screen::debug("\nInitializing x2APIC...");
        _lapic = new (&_x2apic) x2apic;

        _x2apic_enabled = true;
    }

    else if (!x2apic_capable())
    {
        screen::debug("\nInitializing xAPIC...");
        _lapic = new (&_xapic) xapic;
    }

    else
    {
        PANIC("Interrupt remapping has not been enabled, but BSP APIC ID >= 256. Please file a bug report with full"
            " specification of your machine.");
    }

    wrmsr(0xc0000101, (uint64_t)processor::get_current_core());
}

processor::lapic * processor::get_lapic()
{
    return _lapic;
}

bool processor::x2apic_enabled()
{
    return _x2apic_enabled;
}

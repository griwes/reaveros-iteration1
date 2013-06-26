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

#include <processor/lapic.h>
#include <processor/xapic.h>
#include <processor/x2apic.h>

namespace
{
    processor::xapic _xapic;
    processor::x2apic _x2apic;

    processor::lapic * _lapic = nullptr;
}

void processor::lapic::initialize()
{
    new (&_xapic) xapic;

    if (_xapic.x2apic_capable())
    {
        new (&_x2apic) x2apic;

        _lapic = &_x2apic;
    }

    else
    {
        _lapic = &_xapic;
    }
}

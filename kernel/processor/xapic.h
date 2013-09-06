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
#include <utils/mmio.h>

namespace processor
{
    class xapic : public lapic
    {
    public:
        xapic();
        virtual ~xapic() {}

        virtual void ap_initialize();
        virtual uint64_t id();

        virtual void eoi(uint8_t);

        virtual uint32_t current_count();
        virtual uint32_t initial_count();
        virtual void initial_count(uint32_t);
        virtual uint8_t divisor();
        virtual void divisor(uint8_t);
        virtual void set_timer(bool);

        virtual void ipi(uint64_t, ipis, uint8_t = 0);
        virtual void broadcast(broadcasts, ipis, uint8_t = 0);

    private:
        utils::mmio_helper<uint32_t> _register;
    };
}

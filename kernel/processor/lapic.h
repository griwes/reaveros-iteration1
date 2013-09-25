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

#include <time/lapic.h>

namespace processor
{
    enum class ipis
    {
        init,
        sipi,
        generic
    };

    enum class broadcasts
    {
        self = 1,
        all,
        others
    };

    struct isr_context;

    extern "C" bool x2apic_capable();
    bool x2apic_enabled();

    namespace remapping
    {
        inline bool enabled()
        {
            return false;
        }
    }

    class lapic
    {
    public:
        friend class time::lapic::timer;

        static void initialize();

        virtual ~lapic() {}

        virtual void ap_initialize() = 0;
        virtual uint64_t id() = 0;

        virtual void eoi(uint8_t) = 0;

        virtual uint32_t current_count() = 0;
        virtual uint32_t initial_count() = 0;
        virtual void initial_count(uint32_t) = 0;
        virtual uint8_t divisor() = 0;
        virtual void divisor(uint8_t) = 0;
        virtual void set_timer(bool) = 0;

        virtual void ipi(uint64_t, ipis, uint8_t = 0) = 0;
        virtual void broadcast(broadcasts, ipis, uint8_t = 0) = 0;

    protected:
        uint8_t _timer_irq;
        uint8_t _spurious;
    };

    lapic * get_lapic();
}

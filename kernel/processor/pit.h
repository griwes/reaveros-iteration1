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

#include <processor/timer.h>

namespace processor
{
    namespace idt
    {
        struct isr_context;
    }

    namespace _detail
    {
        void _pit_handler(processor::idt::isr_context, uint64_t);
    }

    namespace pit
    {
        void initialize();
        bool ready();

        class timer : public processor::real_timer
        {
        public:
            timer();

            virtual ~timer() {}

        private:
            friend void _detail::_pit_handler(processor::idt::isr_context, uint64_t);

            virtual void _one_shot(uint64_t);
            virtual void _periodic(uint64_t);
            virtual void _update_now();

            static void _init_handler(processor::idt::isr_context, uint64_t context);

            uint8_t _int_vector;
        };
    }
}

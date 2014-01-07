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

#pragma once

#include <time/timer.h>

namespace processor
{
    class lapic;
}

namespace time
{
    namespace lapic
    {
        void initialize();
        void ap_initialize();

        class timer : public time::real_timer
        {
        public:
            timer();
            timer(const timer &);
            timer(decltype(nullptr));
            virtual ~timer() {}

        protected:
            virtual void _one_shot(uint64_t);
            virtual void _periodic(uint64_t);
            virtual void _update_now();
            virtual void _stop();

        private:
            static void _lapic_handler(processor::isr_context &, uint64_t);

            uint64_t _period;
            processor::lapic * _lapic;
        };
    }
}

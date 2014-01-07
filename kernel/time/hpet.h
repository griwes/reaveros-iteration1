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
#include <utils/spinlock.h>
#include <utils/mmio.h>

namespace time
{
    namespace hpet
    {
        void initialize();
        bool ready();

        class timer;

        class comparator : public real_timer
        {
        public:
            comparator();
            comparator(hpet::timer * parent, uint8_t index);

            virtual ~comparator() {}

            bool valid() const
            {
                return _parent;
            }

        protected:
            virtual void _one_shot(uint64_t);
            virtual void _periodic(uint64_t);
            virtual void _update_now();
            virtual void _stop();

        private:
            static void _hpet_handler(processor::isr_context &, uint64_t);

            hpet::timer * _parent;
            uint8_t _index;
            uint8_t _int_vector;
            uint8_t _input;
        };

        class timer : public time::timer
        {
        public:
            friend class comparator;

            timer(uint8_t number, pci_vendor_t pci_vendor, uint64_t address, uint8_t counter_size, uint8_t comparators,
                uint16_t minimal_tick, uint8_t page_protection);

            virtual ~timer() {}

            virtual timer_event_handle one_shot(uint64_t, timer_handler, uint64_t = 0);
            virtual timer_event_handle periodic(uint64_t, timer_handler, uint64_t = 0);
            virtual void cancel(uint64_t);

            uint64_t now();

        private:
            uint8_t _number;
            uint8_t _size;
            uint8_t _comparator_count;
            uint8_t _page_protection;
            pci_vendor_t _pci_vendor;
            uint16_t _minimal_tick;
            uint64_t _maximal_tick;

            uint64_t _period;
            uint64_t _frequency;

            utils::mmio_helper<uint64_t> _register;

            comparator _comparators[32];
        };
    }
}

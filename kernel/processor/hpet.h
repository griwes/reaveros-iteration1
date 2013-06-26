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
#include <utils/spinlock.h>
#include <utils/mmio.h>

namespace processor
{
    namespace hpet
    {
        void initialize();
        bool ready();

        struct timer_description
        {
            timer_description * prev;
            timer_description * next;
            utils::spinlock lock;
            uint64_t id;
        };

        class timer : public processor::timer
        {
        public:
            timer(uint8_t number, pci_vendor_t pci_vendor, uint64_t address, uint8_t counter_size, uint8_t comparators,
                uint16_t minimum_tick, uint8_t page_protection);

            virtual ~timer() {}

            virtual timer_event_handle one_shot(uint64_t, timer_handler);
            virtual timer_event_handle periodic(uint64_t, timer_handler);
            virtual void cancel(uint64_t);

        private:
            uint8_t _number;
            pci_vendor_t _pci_vendor;
            uint8_t _size;
            uint8_t _comparators;
            uint16_t _minimum_tick;
            uint8_t _page_protection;

            utils::mmio_helper<uint64_t> _register;

            timer_description * _active_timers;
            timer_description * _free_descriptors;
        };
    }
}

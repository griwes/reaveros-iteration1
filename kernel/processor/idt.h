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

#include <type_traits>

namespace processor
{
    namespace idt
    {
        struct idt_entry
        {
            uint16_t offset_low;
            uint16_t selector;
            uint8_t ist:3;
            uint8_t zero:5;
            uint8_t type:4;
            uint8_t zero1:1;
            uint8_t dpl:2;
            uint8_t present:1;
            uint16_t offset_middle;
            uint32_t offset_high;
            uint32_t zero2;
        } __attribute__((packed));

        struct idtr
        {
            uint16_t limit;
            idt_entry * address;
        } __attribute__((packed));

        void initialize();
        void ap_initialize();
    }
}

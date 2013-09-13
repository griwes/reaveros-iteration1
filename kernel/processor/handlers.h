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

#include <processor/idt.h>

namespace devices
{
    class device;
}

namespace processor
{
    void initialize_exceptions();

    void handle(idt::isr_context &);

    uint8_t allocate_isr(uint8_t priority, devices::device * owner = nullptr);
    uint8_t allocate_isr(uint8_t priority, uint8_t & count, devices::device * owner = nullptr);
    void free_isr(uint8_t number);
    void register_handler(uint8_t number, void (*)(idt::isr_context &, uint64_t), uint64_t context = 0);
    void unregister_handler(uint8_t number);
    void set_isa_irq_int_vector(uint8_t isa, uint8_t handler);
}

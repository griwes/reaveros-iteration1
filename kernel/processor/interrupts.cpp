/**
 * Reaver Project OS, Rose License
 * 
 * Copyright (C) 2011-2013 Reaver Project Team:
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

#include <processor/interrupts.h>
#include <memory/memory.h>
#include <processor/handlers.h>
#include <processor/current_core.h>

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
            uint64_t base;
        } __attribute__((packed));
    }
}
   
extern "C" processor::idt::idtr idtr;
processor::idt::idtr idtr;
        
namespace
{
    processor::idt::idt_entry _idt[256];
}

namespace processor
{
    namespace idt
    {
        void setup_idte(uint8_t id, uint64_t address, uint16_t selector, bool present, uint8_t dpl, uint8_t type, uint8_t ist = 0)
        {
            _idt[id].zero = 0;
            _idt[id].zero1 = 0;
            _idt[id].zero2 = 0;
            _idt[id].offset_low = address & 0xffff;
            _idt[id].offset_middle = (address >> 16) & 0xffff;
            _idt[id].offset_high = (address >> 32) & 0xffffffff;
            _idt[id].selector = selector;
            _idt[id].present = present;
            _idt[id].dpl = dpl;
            _idt[id].type = type;
            _idt[id].ist = ist;
        }
    }
}

void processor::idt::enable(uint8_t vector)
{
    processor::current_core::unmask(vector);
}

void processor::idt::disable(uint8_t vector)
{
    processor::current_core::mask(vector);
}

extern "C" void _load_idt();

void processor::idt::initialize()
{
    ::idtr.base = (uint64_t)_idt;
    ::idtr.limit = (uint64_t)(_idt + 256) - (uint64_t)_idt - 1;
    
    memory::zero(_idt, 256);
    
    setup_idte(0, (uint64_t)processor::handlers::de, 0x08, true, 0, 0xE);
    setup_idte(1, (uint64_t)processor::handlers::res, 0x08, true, 0, 0xE);
    setup_idte(2, (uint64_t)processor::handlers::nmi, 0x08, true, 0, 0xE); // TODO: add IST
    setup_idte(3, (uint64_t)processor::handlers::rp, 0x08, true, 0, 0xE); // breakpoint
    setup_idte(4, (uint64_t)processor::handlers::of, 0x08, true, 0, 0xE);
    setup_idte(5, (uint64_t)processor::handlers::br, 0x08, true, 0, 0xE);
    setup_idte(6, (uint64_t)processor::handlers::ud, 0x08, true, 0, 0xE);
    setup_idte(7, (uint64_t)processor::handlers::nm, 0x08, true, 0, 0xE);
    setup_idte(8, (uint64_t)processor::handlers::df, 0x08, true, 0, 0xE); // TODO: add IST
    setup_idte(9, (uint64_t)processor::handlers::res, 0x08, true, 0, 0xE);
    setup_idte(10, (uint64_t)processor::handlers::ts, 0x08, true, 0, 0xE);
    setup_idte(11, (uint64_t)processor::handlers::np, 0x08, true, 0, 0xE);
    setup_idte(12, (uint64_t)processor::handlers::sf, 0x08, true, 0, 0xE);
    setup_idte(13, (uint64_t)processor::handlers::gp, 0x08, true, 0, 0xE);
    setup_idte(14, (uint64_t)processor::handlers::pf, 0x08, true, 0, 0xE);
    setup_idte(15, (uint64_t)processor::handlers::res, 0x08, true, 0, 0xE);
    setup_idte(16, (uint64_t)processor::handlers::mf, 0x08, true, 0, 0xE);
    setup_idte(17, (uint64_t)processor::handlers::ac, 0x08, true, 0, 0xE);
    setup_idte(18, (uint64_t)processor::handlers::mc, 0x08, true, 0, 0xE);
    setup_idte(19, (uint64_t)processor::handlers::xm, 0x08, true, 0, 0xE);
    
    for (uint8_t i = 20; i < 32; ++i)
    {
        setup_idte(i, (uint64_t)processor::handlers::res, 0x08, true, 0, 0xE);
    }
    
    _load_idt();
}

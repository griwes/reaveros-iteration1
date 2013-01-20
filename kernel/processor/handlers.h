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

#pragma once

#include <processor/interrupts.h>

namespace processor
{
    namespace handlers
    {
        extern "C" char res[];
        extern "C" char de[];
        extern "C" char nmi[];
        extern "C" char rp[];
        extern "C" char of[];
        extern "C" char br[];
        extern "C" char ud[];
        extern "C" char nm[];
        extern "C" char df[];
        extern "C" char ts[];
        extern "C" char np[];
        extern "C" char sf[];
        extern "C" char gp[];
        extern "C" char pf[];
        extern "C" char mf[];
        extern "C" char ac[];
        extern "C" char mc[];
        extern "C" char xm[];
    }
    
    namespace exceptions
    {
        extern "C" void reserved(idt::irq_context);
        extern "C" void divide_error(idt::irq_context);
        extern "C" void non_maskable(idt::irq_context);
        extern "C" void breakpoint(idt::irq_context);
        extern "C" void overflow(idt::irq_context);
        extern "C" void bound_range(idt::irq_context);
        extern "C" void invalid_opcode(idt::irq_context);
        extern "C" void no_coprocessor(idt::irq_context);
        extern "C" void double_fault(idt::irq_context_error);
        extern "C" void invalid_tss(idt::irq_context_error);
        extern "C" void segment_not_present(idt::irq_context_error);
        extern "C" void stack_fault(idt::irq_context_error);
        extern "C" void protection_fault(idt::irq_context_error);
        extern "C" void page_fault(idt::irq_context_error);
        extern "C" void fpu_error(idt::irq_context);
        extern "C" void alignment_check(idt::irq_context);
        extern "C" void machine_check(idt::irq_context);
        extern "C" void simd_exception(idt::irq_context);
    }
}
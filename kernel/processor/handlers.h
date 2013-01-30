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
        extern "C" void reserved(idt::exc_context);
        extern "C" void divide_error(idt::exc_context);
        extern "C" void non_maskable(idt::exc_context);
        extern "C" void breakpoint(idt::exc_context);
        extern "C" void overflow(idt::exc_context);
        extern "C" void bound_range(idt::exc_context);
        extern "C" void invalid_opcode(idt::exc_context);
        extern "C" void no_coprocessor(idt::exc_context);
        extern "C" void double_fault(idt::exc_context_error);
        extern "C" void invalid_tss(idt::exc_context_error);
        extern "C" void segment_not_present(idt::exc_context_error);
        extern "C" void stack_fault(idt::exc_context_error);
        extern "C" void protection_fault(idt::exc_context_error);
        extern "C" void page_fault(idt::exc_context_error);
        extern "C" void fpu_error(idt::exc_context);
        extern "C" void alignment_check(idt::exc_context);
        extern "C" void machine_check(idt::exc_context);
        extern "C" void simd_exception(idt::exc_context);
    }
    
    namespace interrupts
    {
        using handler = void (*)(processor::idt::irq_context);
        
        extern "C" void common_interrupt_handler(idt::irq_context);
        void set_handler(uint8_t, handler);
        void remove_handler(uint8_t);
        
        extern "C" char irq32[];
        extern "C" char irq33[];
        extern "C" char irq34[];
        extern "C" char irq35[];
        extern "C" char irq36[];
        extern "C" char irq37[];
        extern "C" char irq38[];
        extern "C" char irq39[];
        extern "C" char irq40[];
        extern "C" char irq41[];
        extern "C" char irq42[];
        extern "C" char irq43[];
        extern "C" char irq44[];
        extern "C" char irq45[];
        extern "C" char irq46[];
        extern "C" char irq47[];
        extern "C" char irq48[];
        extern "C" char irq49[];
        extern "C" char irq50[];
        extern "C" char irq51[];
        extern "C" char irq52[];
        extern "C" char irq53[];
        extern "C" char irq54[];
        extern "C" char irq55[];
        extern "C" char irq56[];
        extern "C" char irq57[];
        extern "C" char irq58[];
        extern "C" char irq59[];
        extern "C" char irq60[];
        extern "C" char irq61[];
        extern "C" char irq62[];
        extern "C" char irq63[];
        extern "C" char irq64[];
        extern "C" char irq65[];
        extern "C" char irq66[];
        extern "C" char irq67[];
        extern "C" char irq68[];
        extern "C" char irq69[];
        extern "C" char irq70[];
        extern "C" char irq71[];
        extern "C" char irq72[];
        extern "C" char irq73[];
        extern "C" char irq74[];
        extern "C" char irq75[];
        extern "C" char irq76[];
        extern "C" char irq77[];
        extern "C" char irq78[];
        extern "C" char irq79[];
        extern "C" char irq80[];
        extern "C" char irq81[];
        extern "C" char irq82[];
        extern "C" char irq83[];
        extern "C" char irq84[];
        extern "C" char irq85[];
        extern "C" char irq86[];
        extern "C" char irq87[];
        extern "C" char irq88[];
        extern "C" char irq89[];
        extern "C" char irq90[];
        extern "C" char irq91[];
        extern "C" char irq92[];
        extern "C" char irq93[];
        extern "C" char irq94[];
        extern "C" char irq95[];
        extern "C" char irq96[];
        extern "C" char irq97[];
        extern "C" char irq98[];
        extern "C" char irq99[];
        extern "C" char irq100[];
        extern "C" char irq101[];
        extern "C" char irq102[];
        extern "C" char irq103[];
        extern "C" char irq104[];
        extern "C" char irq105[];
        extern "C" char irq106[];
        extern "C" char irq107[];
        extern "C" char irq108[];
        extern "C" char irq109[];
        extern "C" char irq110[];
        extern "C" char irq111[];
        extern "C" char irq112[];
        extern "C" char irq113[];
        extern "C" char irq114[];
        extern "C" char irq115[];
        extern "C" char irq116[];
        extern "C" char irq117[];
        extern "C" char irq118[];
        extern "C" char irq119[];
        extern "C" char irq120[];
        extern "C" char irq121[];
        extern "C" char irq122[];
        extern "C" char irq123[];
        extern "C" char irq124[];
        extern "C" char irq125[];
        extern "C" char irq126[];
        extern "C" char irq127[];
        extern "C" char irq128[];
        extern "C" char irq129[];
        extern "C" char irq130[];
        extern "C" char irq131[];
        extern "C" char irq132[];
        extern "C" char irq133[];
        extern "C" char irq134[];
        extern "C" char irq135[];
        extern "C" char irq136[];
        extern "C" char irq137[];
        extern "C" char irq138[];
        extern "C" char irq139[];
        extern "C" char irq140[];
        extern "C" char irq141[];
        extern "C" char irq142[];
        extern "C" char irq143[];
        extern "C" char irq144[];
        extern "C" char irq145[];
        extern "C" char irq146[];
        extern "C" char irq147[];
        extern "C" char irq148[];
        extern "C" char irq149[];
        extern "C" char irq150[];
        extern "C" char irq151[];
        extern "C" char irq152[];
        extern "C" char irq153[];
        extern "C" char irq154[];
        extern "C" char irq155[];
        extern "C" char irq156[];
        extern "C" char irq157[];
        extern "C" char irq158[];
        extern "C" char irq159[];
        extern "C" char irq160[];
        extern "C" char irq161[];
        extern "C" char irq162[];
        extern "C" char irq163[];
        extern "C" char irq164[];
        extern "C" char irq165[];
        extern "C" char irq166[];
        extern "C" char irq167[];
        extern "C" char irq168[];
        extern "C" char irq169[];
        extern "C" char irq170[];
        extern "C" char irq171[];
        extern "C" char irq172[];
        extern "C" char irq173[];
        extern "C" char irq174[];
        extern "C" char irq175[];
        extern "C" char irq176[];
        extern "C" char irq177[];
        extern "C" char irq178[];
        extern "C" char irq179[];
        extern "C" char irq180[];
        extern "C" char irq181[];
        extern "C" char irq182[];
        extern "C" char irq183[];
        extern "C" char irq184[];
        extern "C" char irq185[];
        extern "C" char irq186[];
        extern "C" char irq187[];
        extern "C" char irq188[];
        extern "C" char irq189[];
        extern "C" char irq190[];
        extern "C" char irq191[];
        extern "C" char irq192[];
        extern "C" char irq193[];
        extern "C" char irq194[];
        extern "C" char irq195[];
        extern "C" char irq196[];
        extern "C" char irq197[];
        extern "C" char irq198[];
        extern "C" char irq199[];
        extern "C" char irq200[];
        extern "C" char irq201[];
        extern "C" char irq202[];
        extern "C" char irq203[];
        extern "C" char irq204[];
        extern "C" char irq205[];
        extern "C" char irq206[];
        extern "C" char irq207[];
        extern "C" char irq208[];
        extern "C" char irq209[];
        extern "C" char irq210[];
        extern "C" char irq211[];
        extern "C" char irq212[];
        extern "C" char irq213[];
        extern "C" char irq214[];
        extern "C" char irq215[];
        extern "C" char irq216[];
        extern "C" char irq217[];
        extern "C" char irq218[];
        extern "C" char irq219[];
        extern "C" char irq220[];
        extern "C" char irq221[];
        extern "C" char irq222[];
        extern "C" char irq223[];
        extern "C" char irq224[];
        extern "C" char irq225[];
        extern "C" char irq226[];
        extern "C" char irq227[];
        extern "C" char irq228[];
        extern "C" char irq229[];
        extern "C" char irq230[];
        extern "C" char irq231[];
        extern "C" char irq232[];
        extern "C" char irq233[];
        extern "C" char irq234[];
        extern "C" char irq235[];
        extern "C" char irq236[];
        extern "C" char irq237[];
        extern "C" char irq238[];
        extern "C" char irq239[];
        extern "C" char irq240[];
        extern "C" char irq241[];
        extern "C" char irq242[];
        extern "C" char irq243[];
        extern "C" char irq244[];
        extern "C" char irq245[];
        extern "C" char irq246[];
        extern "C" char irq247[];
        extern "C" char irq248[];
        extern "C" char irq249[];
        extern "C" char irq250[];
        extern "C" char irq251[];
        extern "C" char irq252[];
        extern "C" char irq253[];
        extern "C" char irq254[];
        extern "C" char irq255[];
    }
}
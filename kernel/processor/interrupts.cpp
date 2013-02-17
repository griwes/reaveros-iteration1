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

extern "C" processor::idt::idtr idtr;
processor::idt::idtr idtr;
        
namespace
{
    processor::idt::idt_entry _idt[256];
}

void processor::idt::enable(uint8_t /*vector*/)
{
//    processor::current_core::unmask(vector);
}

void processor::idt::disable(uint8_t /*vector*/)
{
//    processor::current_core::mask(vector);
}

extern "C" void _load_idt_from(processor::idt::idtr *);

namespace
{
    void _setup_idte(uint8_t id, uint64_t address, uint16_t selector, bool present, uint8_t dpl, uint8_t type, processor::idt::idt_entry * table, uint8_t ist = 0)
    {
        table[id].zero = 0;
        table[id].zero1 = 0;
        table[id].zero2 = 0;
        table[id].offset_low = address & 0xffff;
        table[id].offset_middle = (address >> 16) & 0xffff;
        table[id].offset_high = (address >> 32) & 0xffffffff;
        table[id].selector = selector;
        table[id].present = present;
        table[id].dpl = dpl;
        table[id].type = type;
        table[id].ist = ist;
    }
    
    void _init(processor::idt::idtr * idtr, processor::idt::idt_entry * idt)
    {
        idtr->base = (uint64_t)idt;
        idtr->limit = (uint64_t)(idt + 256) - (uint64_t)idt - 1;
        
        memory::zero(idt, 256);
        
        _setup_idte(0, (uint64_t)processor::handlers::de, 0x08, true, 0, 0xE, idt);
        _setup_idte(1, (uint64_t)processor::handlers::res, 0x08, true, 0, 0xE, idt);
        _setup_idte(2, (uint64_t)processor::handlers::nmi, 0x08, true, 0, 0xE, idt, 1);
        _setup_idte(3, (uint64_t)processor::handlers::rp, 0x08, true, 0, 0xE, idt); // breakpoint
        _setup_idte(4, (uint64_t)processor::handlers::of, 0x08, true, 0, 0xE, idt);
        _setup_idte(5, (uint64_t)processor::handlers::br, 0x08, true, 0, 0xE, idt);
        _setup_idte(6, (uint64_t)processor::handlers::ud, 0x08, true, 0, 0xE, idt);
        _setup_idte(7, (uint64_t)processor::handlers::nm, 0x08, true, 0, 0xE, idt);
        _setup_idte(8, (uint64_t)processor::handlers::df, 0x08, true, 0, 0xE, idt, 2);
        _setup_idte(9, (uint64_t)processor::handlers::res, 0x08, true, 0, 0xE, idt);
        _setup_idte(10, (uint64_t)processor::handlers::ts, 0x08, true, 0, 0xE, idt);
        _setup_idte(11, (uint64_t)processor::handlers::np, 0x08, true, 0, 0xE, idt);
        _setup_idte(12, (uint64_t)processor::handlers::sf, 0x08, true, 0, 0xE, idt);
        _setup_idte(13, (uint64_t)processor::handlers::gp, 0x08, true, 0, 0xE, idt);
        _setup_idte(14, (uint64_t)processor::handlers::pf, 0x08, true, 0, 0xE, idt, 3);
        _setup_idte(15, (uint64_t)processor::handlers::res, 0x08, true, 0, 0xE, idt);
        _setup_idte(16, (uint64_t)processor::handlers::mf, 0x08, true, 0, 0xE, idt);
        _setup_idte(17, (uint64_t)processor::handlers::ac, 0x08, true, 0, 0xE, idt);
        _setup_idte(18, (uint64_t)processor::handlers::mc, 0x08, true, 0, 0xE, idt);
        _setup_idte(19, (uint64_t)processor::handlers::xm, 0x08, true, 0, 0xE, idt);
        
        for (uint8_t i = 20; i < 32; ++i)
        {
            _setup_idte(i, (uint64_t)processor::handlers::res, 0x08, true, 0, 0xE, idt);
        }
        
        _setup_idte(32, (uint64_t)processor::interrupts::irq32, 0x08, true, 0, 0xE, idt);
        _setup_idte(33, (uint64_t)processor::interrupts::irq33, 0x08, true, 0, 0xE, idt);
        _setup_idte(34, (uint64_t)processor::interrupts::irq34, 0x08, true, 0, 0xE, idt);
        _setup_idte(35, (uint64_t)processor::interrupts::irq35, 0x08, true, 0, 0xE, idt);
        _setup_idte(36, (uint64_t)processor::interrupts::irq36, 0x08, true, 0, 0xE, idt);
        _setup_idte(37, (uint64_t)processor::interrupts::irq37, 0x08, true, 0, 0xE, idt);
        _setup_idte(38, (uint64_t)processor::interrupts::irq38, 0x08, true, 0, 0xE, idt);
        _setup_idte(39, (uint64_t)processor::interrupts::irq39, 0x08, true, 0, 0xE, idt);
        _setup_idte(40, (uint64_t)processor::interrupts::irq40, 0x08, true, 0, 0xE, idt);
        _setup_idte(41, (uint64_t)processor::interrupts::irq41, 0x08, true, 0, 0xE, idt);
        _setup_idte(42, (uint64_t)processor::interrupts::irq42, 0x08, true, 0, 0xE, idt);
        _setup_idte(43, (uint64_t)processor::interrupts::irq43, 0x08, true, 0, 0xE, idt);
        _setup_idte(44, (uint64_t)processor::interrupts::irq44, 0x08, true, 0, 0xE, idt);
        _setup_idte(45, (uint64_t)processor::interrupts::irq45, 0x08, true, 0, 0xE, idt);
        _setup_idte(46, (uint64_t)processor::interrupts::irq46, 0x08, true, 0, 0xE, idt);
        _setup_idte(47, (uint64_t)processor::interrupts::irq47, 0x08, true, 0, 0xE, idt);
        _setup_idte(48, (uint64_t)processor::interrupts::irq48, 0x08, true, 0, 0xE, idt);
        _setup_idte(49, (uint64_t)processor::interrupts::irq49, 0x08, true, 0, 0xE, idt);
        _setup_idte(50, (uint64_t)processor::interrupts::irq50, 0x08, true, 0, 0xE, idt);
        _setup_idte(51, (uint64_t)processor::interrupts::irq51, 0x08, true, 0, 0xE, idt);
        _setup_idte(52, (uint64_t)processor::interrupts::irq52, 0x08, true, 0, 0xE, idt);
        _setup_idte(53, (uint64_t)processor::interrupts::irq53, 0x08, true, 0, 0xE, idt);
        _setup_idte(54, (uint64_t)processor::interrupts::irq54, 0x08, true, 0, 0xE, idt);
        _setup_idte(55, (uint64_t)processor::interrupts::irq55, 0x08, true, 0, 0xE, idt);
        _setup_idte(56, (uint64_t)processor::interrupts::irq56, 0x08, true, 0, 0xE, idt);
        _setup_idte(57, (uint64_t)processor::interrupts::irq57, 0x08, true, 0, 0xE, idt);
        _setup_idte(58, (uint64_t)processor::interrupts::irq58, 0x08, true, 0, 0xE, idt);
        _setup_idte(59, (uint64_t)processor::interrupts::irq59, 0x08, true, 0, 0xE, idt);
        _setup_idte(60, (uint64_t)processor::interrupts::irq60, 0x08, true, 0, 0xE, idt);
        _setup_idte(61, (uint64_t)processor::interrupts::irq61, 0x08, true, 0, 0xE, idt);
        _setup_idte(62, (uint64_t)processor::interrupts::irq62, 0x08, true, 0, 0xE, idt);
        _setup_idte(63, (uint64_t)processor::interrupts::irq63, 0x08, true, 0, 0xE, idt);
        _setup_idte(64, (uint64_t)processor::interrupts::irq64, 0x08, true, 0, 0xE, idt);
        _setup_idte(65, (uint64_t)processor::interrupts::irq65, 0x08, true, 0, 0xE, idt);
        _setup_idte(66, (uint64_t)processor::interrupts::irq66, 0x08, true, 0, 0xE, idt);
        _setup_idte(67, (uint64_t)processor::interrupts::irq67, 0x08, true, 0, 0xE, idt);
        _setup_idte(68, (uint64_t)processor::interrupts::irq68, 0x08, true, 0, 0xE, idt);
        _setup_idte(69, (uint64_t)processor::interrupts::irq69, 0x08, true, 0, 0xE, idt);
        _setup_idte(70, (uint64_t)processor::interrupts::irq70, 0x08, true, 0, 0xE, idt);
        _setup_idte(71, (uint64_t)processor::interrupts::irq71, 0x08, true, 0, 0xE, idt);
        _setup_idte(72, (uint64_t)processor::interrupts::irq72, 0x08, true, 0, 0xE, idt);
        _setup_idte(73, (uint64_t)processor::interrupts::irq73, 0x08, true, 0, 0xE, idt);
        _setup_idte(74, (uint64_t)processor::interrupts::irq74, 0x08, true, 0, 0xE, idt);
        _setup_idte(75, (uint64_t)processor::interrupts::irq75, 0x08, true, 0, 0xE, idt);
        _setup_idte(76, (uint64_t)processor::interrupts::irq76, 0x08, true, 0, 0xE, idt);
        _setup_idte(77, (uint64_t)processor::interrupts::irq77, 0x08, true, 0, 0xE, idt);
        _setup_idte(78, (uint64_t)processor::interrupts::irq78, 0x08, true, 0, 0xE, idt);
        _setup_idte(79, (uint64_t)processor::interrupts::irq79, 0x08, true, 0, 0xE, idt);
        _setup_idte(80, (uint64_t)processor::interrupts::irq80, 0x08, true, 0, 0xE, idt);
        _setup_idte(81, (uint64_t)processor::interrupts::irq81, 0x08, true, 0, 0xE, idt);
        _setup_idte(82, (uint64_t)processor::interrupts::irq82, 0x08, true, 0, 0xE, idt);
        _setup_idte(83, (uint64_t)processor::interrupts::irq83, 0x08, true, 0, 0xE, idt);
        _setup_idte(84, (uint64_t)processor::interrupts::irq84, 0x08, true, 0, 0xE, idt);
        _setup_idte(85, (uint64_t)processor::interrupts::irq85, 0x08, true, 0, 0xE, idt);
        _setup_idte(86, (uint64_t)processor::interrupts::irq86, 0x08, true, 0, 0xE, idt);
        _setup_idte(87, (uint64_t)processor::interrupts::irq87, 0x08, true, 0, 0xE, idt);
        _setup_idte(88, (uint64_t)processor::interrupts::irq88, 0x08, true, 0, 0xE, idt);
        _setup_idte(89, (uint64_t)processor::interrupts::irq89, 0x08, true, 0, 0xE, idt);
        _setup_idte(90, (uint64_t)processor::interrupts::irq90, 0x08, true, 0, 0xE, idt);
        _setup_idte(91, (uint64_t)processor::interrupts::irq91, 0x08, true, 0, 0xE, idt);
        _setup_idte(92, (uint64_t)processor::interrupts::irq92, 0x08, true, 0, 0xE, idt);
        _setup_idte(93, (uint64_t)processor::interrupts::irq93, 0x08, true, 0, 0xE, idt);
        _setup_idte(94, (uint64_t)processor::interrupts::irq94, 0x08, true, 0, 0xE, idt);
        _setup_idte(95, (uint64_t)processor::interrupts::irq95, 0x08, true, 0, 0xE, idt);
        _setup_idte(96, (uint64_t)processor::interrupts::irq96, 0x08, true, 0, 0xE, idt);
        _setup_idte(97, (uint64_t)processor::interrupts::irq97, 0x08, true, 0, 0xE, idt);
        _setup_idte(98, (uint64_t)processor::interrupts::irq98, 0x08, true, 0, 0xE, idt);
        _setup_idte(99, (uint64_t)processor::interrupts::irq99, 0x08, true, 0, 0xE, idt);
        _setup_idte(100, (uint64_t)processor::interrupts::irq100, 0x08, true, 0, 0xE, idt);
        _setup_idte(101, (uint64_t)processor::interrupts::irq101, 0x08, true, 0, 0xE, idt);
        _setup_idte(102, (uint64_t)processor::interrupts::irq102, 0x08, true, 0, 0xE, idt);
        _setup_idte(103, (uint64_t)processor::interrupts::irq103, 0x08, true, 0, 0xE, idt);
        _setup_idte(104, (uint64_t)processor::interrupts::irq104, 0x08, true, 0, 0xE, idt);
        _setup_idte(105, (uint64_t)processor::interrupts::irq105, 0x08, true, 0, 0xE, idt);
        _setup_idte(106, (uint64_t)processor::interrupts::irq106, 0x08, true, 0, 0xE, idt);
        _setup_idte(107, (uint64_t)processor::interrupts::irq107, 0x08, true, 0, 0xE, idt);
        _setup_idte(108, (uint64_t)processor::interrupts::irq108, 0x08, true, 0, 0xE, idt);
        _setup_idte(109, (uint64_t)processor::interrupts::irq109, 0x08, true, 0, 0xE, idt);
        _setup_idte(110, (uint64_t)processor::interrupts::irq110, 0x08, true, 0, 0xE, idt);
        _setup_idte(111, (uint64_t)processor::interrupts::irq111, 0x08, true, 0, 0xE, idt);
        _setup_idte(112, (uint64_t)processor::interrupts::irq112, 0x08, true, 0, 0xE, idt);
        _setup_idte(113, (uint64_t)processor::interrupts::irq113, 0x08, true, 0, 0xE, idt);
        _setup_idte(114, (uint64_t)processor::interrupts::irq114, 0x08, true, 0, 0xE, idt);
        _setup_idte(115, (uint64_t)processor::interrupts::irq115, 0x08, true, 0, 0xE, idt);
        _setup_idte(116, (uint64_t)processor::interrupts::irq116, 0x08, true, 0, 0xE, idt);
        _setup_idte(117, (uint64_t)processor::interrupts::irq117, 0x08, true, 0, 0xE, idt);
        _setup_idte(118, (uint64_t)processor::interrupts::irq118, 0x08, true, 0, 0xE, idt);
        _setup_idte(119, (uint64_t)processor::interrupts::irq119, 0x08, true, 0, 0xE, idt);
        _setup_idte(120, (uint64_t)processor::interrupts::irq120, 0x08, true, 0, 0xE, idt);
        _setup_idte(121, (uint64_t)processor::interrupts::irq121, 0x08, true, 0, 0xE, idt);
        _setup_idte(122, (uint64_t)processor::interrupts::irq122, 0x08, true, 0, 0xE, idt);
        _setup_idte(123, (uint64_t)processor::interrupts::irq123, 0x08, true, 0, 0xE, idt);
        _setup_idte(124, (uint64_t)processor::interrupts::irq124, 0x08, true, 0, 0xE, idt);
        _setup_idte(125, (uint64_t)processor::interrupts::irq125, 0x08, true, 0, 0xE, idt);
        _setup_idte(126, (uint64_t)processor::interrupts::irq126, 0x08, true, 0, 0xE, idt);
        _setup_idte(127, (uint64_t)processor::interrupts::irq127, 0x08, true, 0, 0xE, idt);
        _setup_idte(128, (uint64_t)processor::interrupts::irq128, 0x08, true, 0, 0xE, idt);
        _setup_idte(129, (uint64_t)processor::interrupts::irq129, 0x08, true, 0, 0xE, idt);
        _setup_idte(130, (uint64_t)processor::interrupts::irq130, 0x08, true, 0, 0xE, idt);
        _setup_idte(131, (uint64_t)processor::interrupts::irq131, 0x08, true, 0, 0xE, idt);
        _setup_idte(132, (uint64_t)processor::interrupts::irq132, 0x08, true, 0, 0xE, idt);
        _setup_idte(133, (uint64_t)processor::interrupts::irq133, 0x08, true, 0, 0xE, idt);
        _setup_idte(134, (uint64_t)processor::interrupts::irq134, 0x08, true, 0, 0xE, idt);
        _setup_idte(135, (uint64_t)processor::interrupts::irq135, 0x08, true, 0, 0xE, idt);
        _setup_idte(136, (uint64_t)processor::interrupts::irq136, 0x08, true, 0, 0xE, idt);
        _setup_idte(137, (uint64_t)processor::interrupts::irq137, 0x08, true, 0, 0xE, idt);
        _setup_idte(138, (uint64_t)processor::interrupts::irq138, 0x08, true, 0, 0xE, idt);
        _setup_idte(139, (uint64_t)processor::interrupts::irq139, 0x08, true, 0, 0xE, idt);
        _setup_idte(140, (uint64_t)processor::interrupts::irq140, 0x08, true, 0, 0xE, idt);
        _setup_idte(141, (uint64_t)processor::interrupts::irq141, 0x08, true, 0, 0xE, idt);
        _setup_idte(142, (uint64_t)processor::interrupts::irq142, 0x08, true, 0, 0xE, idt);
        _setup_idte(143, (uint64_t)processor::interrupts::irq143, 0x08, true, 0, 0xE, idt);
        _setup_idte(144, (uint64_t)processor::interrupts::irq144, 0x08, true, 0, 0xE, idt);
        _setup_idte(145, (uint64_t)processor::interrupts::irq145, 0x08, true, 0, 0xE, idt);
        _setup_idte(146, (uint64_t)processor::interrupts::irq146, 0x08, true, 0, 0xE, idt);
        _setup_idte(147, (uint64_t)processor::interrupts::irq147, 0x08, true, 0, 0xE, idt);
        _setup_idte(148, (uint64_t)processor::interrupts::irq148, 0x08, true, 0, 0xE, idt);
        _setup_idte(149, (uint64_t)processor::interrupts::irq149, 0x08, true, 0, 0xE, idt);
        _setup_idte(150, (uint64_t)processor::interrupts::irq150, 0x08, true, 0, 0xE, idt);
        _setup_idte(151, (uint64_t)processor::interrupts::irq151, 0x08, true, 0, 0xE, idt);
        _setup_idte(152, (uint64_t)processor::interrupts::irq152, 0x08, true, 0, 0xE, idt);
        _setup_idte(153, (uint64_t)processor::interrupts::irq153, 0x08, true, 0, 0xE, idt);
        _setup_idte(154, (uint64_t)processor::interrupts::irq154, 0x08, true, 0, 0xE, idt);
        _setup_idte(155, (uint64_t)processor::interrupts::irq155, 0x08, true, 0, 0xE, idt);
        _setup_idte(156, (uint64_t)processor::interrupts::irq156, 0x08, true, 0, 0xE, idt);
        _setup_idte(157, (uint64_t)processor::interrupts::irq157, 0x08, true, 0, 0xE, idt);
        _setup_idte(158, (uint64_t)processor::interrupts::irq158, 0x08, true, 0, 0xE, idt);
        _setup_idte(159, (uint64_t)processor::interrupts::irq159, 0x08, true, 0, 0xE, idt);
        _setup_idte(160, (uint64_t)processor::interrupts::irq160, 0x08, true, 0, 0xE, idt);
        _setup_idte(161, (uint64_t)processor::interrupts::irq161, 0x08, true, 0, 0xE, idt);
        _setup_idte(162, (uint64_t)processor::interrupts::irq162, 0x08, true, 0, 0xE, idt);
        _setup_idte(163, (uint64_t)processor::interrupts::irq163, 0x08, true, 0, 0xE, idt);
        _setup_idte(164, (uint64_t)processor::interrupts::irq164, 0x08, true, 0, 0xE, idt);
        _setup_idte(165, (uint64_t)processor::interrupts::irq165, 0x08, true, 0, 0xE, idt);
        _setup_idte(166, (uint64_t)processor::interrupts::irq166, 0x08, true, 0, 0xE, idt);
        _setup_idte(167, (uint64_t)processor::interrupts::irq167, 0x08, true, 0, 0xE, idt);
        _setup_idte(168, (uint64_t)processor::interrupts::irq168, 0x08, true, 0, 0xE, idt);
        _setup_idte(169, (uint64_t)processor::interrupts::irq169, 0x08, true, 0, 0xE, idt);
        _setup_idte(170, (uint64_t)processor::interrupts::irq170, 0x08, true, 0, 0xE, idt);
        _setup_idte(171, (uint64_t)processor::interrupts::irq171, 0x08, true, 0, 0xE, idt);
        _setup_idte(172, (uint64_t)processor::interrupts::irq172, 0x08, true, 0, 0xE, idt);
        _setup_idte(173, (uint64_t)processor::interrupts::irq173, 0x08, true, 0, 0xE, idt);
        _setup_idte(174, (uint64_t)processor::interrupts::irq174, 0x08, true, 0, 0xE, idt);
        _setup_idte(175, (uint64_t)processor::interrupts::irq175, 0x08, true, 0, 0xE, idt);
        _setup_idte(176, (uint64_t)processor::interrupts::irq176, 0x08, true, 0, 0xE, idt);
        _setup_idte(177, (uint64_t)processor::interrupts::irq177, 0x08, true, 0, 0xE, idt);
        _setup_idte(178, (uint64_t)processor::interrupts::irq178, 0x08, true, 0, 0xE, idt);
        _setup_idte(179, (uint64_t)processor::interrupts::irq179, 0x08, true, 0, 0xE, idt);
        _setup_idte(180, (uint64_t)processor::interrupts::irq180, 0x08, true, 0, 0xE, idt);
        _setup_idte(181, (uint64_t)processor::interrupts::irq181, 0x08, true, 0, 0xE, idt);
        _setup_idte(182, (uint64_t)processor::interrupts::irq182, 0x08, true, 0, 0xE, idt);
        _setup_idte(183, (uint64_t)processor::interrupts::irq183, 0x08, true, 0, 0xE, idt);
        _setup_idte(184, (uint64_t)processor::interrupts::irq184, 0x08, true, 0, 0xE, idt);
        _setup_idte(185, (uint64_t)processor::interrupts::irq185, 0x08, true, 0, 0xE, idt);
        _setup_idte(186, (uint64_t)processor::interrupts::irq186, 0x08, true, 0, 0xE, idt);
        _setup_idte(187, (uint64_t)processor::interrupts::irq187, 0x08, true, 0, 0xE, idt);
        _setup_idte(188, (uint64_t)processor::interrupts::irq188, 0x08, true, 0, 0xE, idt);
        _setup_idte(189, (uint64_t)processor::interrupts::irq189, 0x08, true, 0, 0xE, idt);
        _setup_idte(190, (uint64_t)processor::interrupts::irq190, 0x08, true, 0, 0xE, idt);
        _setup_idte(191, (uint64_t)processor::interrupts::irq191, 0x08, true, 0, 0xE, idt);
        _setup_idte(192, (uint64_t)processor::interrupts::irq192, 0x08, true, 0, 0xE, idt);
        _setup_idte(193, (uint64_t)processor::interrupts::irq193, 0x08, true, 0, 0xE, idt);
        _setup_idte(194, (uint64_t)processor::interrupts::irq194, 0x08, true, 0, 0xE, idt);
        _setup_idte(195, (uint64_t)processor::interrupts::irq195, 0x08, true, 0, 0xE, idt);
        _setup_idte(196, (uint64_t)processor::interrupts::irq196, 0x08, true, 0, 0xE, idt);
        _setup_idte(197, (uint64_t)processor::interrupts::irq197, 0x08, true, 0, 0xE, idt);
        _setup_idte(198, (uint64_t)processor::interrupts::irq198, 0x08, true, 0, 0xE, idt);
        _setup_idte(199, (uint64_t)processor::interrupts::irq199, 0x08, true, 0, 0xE, idt);
        _setup_idte(200, (uint64_t)processor::interrupts::irq200, 0x08, true, 0, 0xE, idt);
        _setup_idte(201, (uint64_t)processor::interrupts::irq201, 0x08, true, 0, 0xE, idt);
        _setup_idte(202, (uint64_t)processor::interrupts::irq202, 0x08, true, 0, 0xE, idt);
        _setup_idte(203, (uint64_t)processor::interrupts::irq203, 0x08, true, 0, 0xE, idt);
        _setup_idte(204, (uint64_t)processor::interrupts::irq204, 0x08, true, 0, 0xE, idt);
        _setup_idte(205, (uint64_t)processor::interrupts::irq205, 0x08, true, 0, 0xE, idt);
        _setup_idte(206, (uint64_t)processor::interrupts::irq206, 0x08, true, 0, 0xE, idt);
        _setup_idte(207, (uint64_t)processor::interrupts::irq207, 0x08, true, 0, 0xE, idt);
        _setup_idte(208, (uint64_t)processor::interrupts::irq208, 0x08, true, 0, 0xE, idt);
        _setup_idte(209, (uint64_t)processor::interrupts::irq209, 0x08, true, 0, 0xE, idt);
        _setup_idte(210, (uint64_t)processor::interrupts::irq210, 0x08, true, 0, 0xE, idt);
        _setup_idte(211, (uint64_t)processor::interrupts::irq211, 0x08, true, 0, 0xE, idt);
        _setup_idte(212, (uint64_t)processor::interrupts::irq212, 0x08, true, 0, 0xE, idt);
        _setup_idte(213, (uint64_t)processor::interrupts::irq213, 0x08, true, 0, 0xE, idt);
        _setup_idte(214, (uint64_t)processor::interrupts::irq214, 0x08, true, 0, 0xE, idt);
        _setup_idte(215, (uint64_t)processor::interrupts::irq215, 0x08, true, 0, 0xE, idt);
        _setup_idte(216, (uint64_t)processor::interrupts::irq216, 0x08, true, 0, 0xE, idt);
        _setup_idte(217, (uint64_t)processor::interrupts::irq217, 0x08, true, 0, 0xE, idt);
        _setup_idte(218, (uint64_t)processor::interrupts::irq218, 0x08, true, 0, 0xE, idt);
        _setup_idte(219, (uint64_t)processor::interrupts::irq219, 0x08, true, 0, 0xE, idt);
        _setup_idte(220, (uint64_t)processor::interrupts::irq220, 0x08, true, 0, 0xE, idt);
        _setup_idte(221, (uint64_t)processor::interrupts::irq221, 0x08, true, 0, 0xE, idt);
        _setup_idte(222, (uint64_t)processor::interrupts::irq222, 0x08, true, 0, 0xE, idt);
        _setup_idte(223, (uint64_t)processor::interrupts::irq223, 0x08, true, 0, 0xE, idt);
        _setup_idte(224, (uint64_t)processor::interrupts::irq224, 0x08, true, 0, 0xE, idt);
        _setup_idte(225, (uint64_t)processor::interrupts::irq225, 0x08, true, 0, 0xE, idt);
        _setup_idte(226, (uint64_t)processor::interrupts::irq226, 0x08, true, 0, 0xE, idt);
        _setup_idte(227, (uint64_t)processor::interrupts::irq227, 0x08, true, 0, 0xE, idt);
        _setup_idte(228, (uint64_t)processor::interrupts::irq228, 0x08, true, 0, 0xE, idt);
        _setup_idte(229, (uint64_t)processor::interrupts::irq229, 0x08, true, 0, 0xE, idt);
        _setup_idte(230, (uint64_t)processor::interrupts::irq230, 0x08, true, 0, 0xE, idt);
        _setup_idte(231, (uint64_t)processor::interrupts::irq231, 0x08, true, 0, 0xE, idt);
        _setup_idte(232, (uint64_t)processor::interrupts::irq232, 0x08, true, 0, 0xE, idt);
        _setup_idte(233, (uint64_t)processor::interrupts::irq233, 0x08, true, 0, 0xE, idt);
        _setup_idte(234, (uint64_t)processor::interrupts::irq234, 0x08, true, 0, 0xE, idt);
        _setup_idte(235, (uint64_t)processor::interrupts::irq235, 0x08, true, 0, 0xE, idt);
        _setup_idte(236, (uint64_t)processor::interrupts::irq236, 0x08, true, 0, 0xE, idt);
        _setup_idte(237, (uint64_t)processor::interrupts::irq237, 0x08, true, 0, 0xE, idt);
        _setup_idte(238, (uint64_t)processor::interrupts::irq238, 0x08, true, 0, 0xE, idt);
        _setup_idte(239, (uint64_t)processor::interrupts::irq239, 0x08, true, 0, 0xE, idt);
        _setup_idte(240, (uint64_t)processor::interrupts::irq240, 0x08, true, 0, 0xE, idt);
        _setup_idte(241, (uint64_t)processor::interrupts::irq241, 0x08, true, 0, 0xE, idt);
        _setup_idte(242, (uint64_t)processor::interrupts::irq242, 0x08, true, 0, 0xE, idt);
        _setup_idte(243, (uint64_t)processor::interrupts::irq243, 0x08, true, 0, 0xE, idt);
        _setup_idte(244, (uint64_t)processor::interrupts::irq244, 0x08, true, 0, 0xE, idt);
        _setup_idte(245, (uint64_t)processor::interrupts::irq245, 0x08, true, 0, 0xE, idt);
        _setup_idte(246, (uint64_t)processor::interrupts::irq246, 0x08, true, 0, 0xE, idt);
        _setup_idte(247, (uint64_t)processor::interrupts::irq247, 0x08, true, 0, 0xE, idt);
        _setup_idte(248, (uint64_t)processor::interrupts::irq248, 0x08, true, 0, 0xE, idt);
        _setup_idte(249, (uint64_t)processor::interrupts::irq249, 0x08, true, 0, 0xE, idt);
        _setup_idte(250, (uint64_t)processor::interrupts::irq250, 0x08, true, 0, 0xE, idt);
        _setup_idte(251, (uint64_t)processor::interrupts::irq251, 0x08, true, 0, 0xE, idt);
        _setup_idte(252, (uint64_t)processor::interrupts::irq252, 0x08, true, 0, 0xE, idt);
        _setup_idte(253, (uint64_t)processor::interrupts::irq253, 0x08, true, 0, 0xE, idt);
        _setup_idte(254, (uint64_t)processor::interrupts::irq254, 0x08, true, 0, 0xE, idt);
        _setup_idte(255, (uint64_t)processor::interrupts::irq255, 0x08, true, 0, 0xE, idt);
        
        _load_idt_from(idtr);
    }
}

void processor::idt::initialize()
{
    _init(&::idtr, _idt);
}

void processor::idt::ap_initialize(processor::idt::idtr * idtr, processor::idt::idt_entry * idt)
{
    _init(idtr, idt);
}

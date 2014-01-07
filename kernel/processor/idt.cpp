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

#include <reaver/static_for.h>

#include <processor/idt.h>
#include <screen/screen.h>
#include <processor/handlers.h>

namespace
{
    processor::idt::idt_entry _idt[256];
    processor::idt::idtr _idtr;

    extern "C" void _common_interrupt_handler(processor::isr_context context)
    {
        processor::handle(context);
    }

    extern "C" void common_interrupt_stub();

    // no error code
    template<uint64_t I, typename std::enable_if<I != 8 && (I < 10 || I > 14) && I != 17, int>::type = 0>
    __attribute__((naked)) void _isr()
    {
        asm volatile (
            R"(
                push    $0
                push    %0

                jmp     common_interrupt_stub
            )" :: "i"(I)
        );
    }

    // error code
    template<uint64_t I, typename std::enable_if<I == 8 || (I >= 10 && I <= 14) || I == 17, int>::type = 0>
    __attribute__((naked)) void _isr()
    {
        asm volatile (
            R"(
                push    %0

                jmp     common_interrupt_stub
            )" :: "i"(I)
        );
    }

    void _setup_idte(uint8_t id, void (*fun)(), uint16_t selector, bool present, uint8_t dpl, uint8_t type,
        processor::idt::idt_entry * table, uint8_t ist = 0)
    {
        uint64_t address = (uint64_t)fun;

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

    template<uint64_t I>
    struct _setup_isr
    {
        void operator()()
        {
            _setup_idte(I, &_isr<I>, 0x8, true, 0, 0xE, _idt);
        }
    };

    template<>
    struct _setup_isr<2>
    {
        void operator()()
        {
            _setup_idte(2, &_isr<2>, 0x8, true, 0, 0xE, _idt, 1);
        }
    };

    template<>
    struct _setup_isr<8>
    {
        void operator()()
        {
            _setup_idte(8, &_isr<8>, 0x8, true, 0, 0xE, _idt, 2);
        }
    };

    template<>
    struct _setup_isr<14>
    {
        void operator()()
        {
            _setup_idte(14, &_isr<14>, 0x8, true, 0, 0xE, _idt, 3);
        }
    };
}

extern "C" void load_idt(processor::idt::idtr *);

void processor::idt::initialize()
{
    reaver::static_for<0, 256, _setup_isr>::exec();

    _idtr.address = _idt;
    _idtr.limit = 256 * sizeof(idt_entry) - 1;

    load_idt(&_idtr);
}

void processor::idt::ap_initialize()
{
    load_idt(&_idtr);
}

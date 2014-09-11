/**
 * Reaver Project OS, Rose License
 *
 * Copyright © 2013-2014 Michał "Griwes" Dominiak
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

#include <processor/gdt.h>
#include <memory/memory.h>
#include <memory/vm.h>
#include <memory/pmm.h>
#include <processor/processor.h>
#include <processor/core.h>

#include <screen/screen.h>

processor::gdt::tss _tss;

namespace
{
    processor::gdt::gdt_entry _gdt[7];
    processor::gdt::gdtr _gdtr;

    void _setup_gdte(uint64_t id, bool code, bool user, processor::gdt::gdt_entry * start = _gdt)
    {
        start[id].normal = 1;

        start[id].code = code;
        start[id].dpl = user * 3;
        start[id].long_mode = 1;
        start[id].present = 1;
        start[id].read_write = 1;
    }

    void _setup_tss(uint64_t id, processor::gdt::gdt_entry * start = _gdt, processor::gdt::tss * tss = &_tss)
    {
        memory::zero(tss);

        tss->iomap = sizeof(processor::gdt::tss);

        start[id].base_low = (uint64_t)tss & 0xFFFFFF;
        start[id].base_high = ((uint64_t)tss >> 24) & 0xFF;
        *(uint32_t *)&start[id + 1] = ((uint64_t)tss >> 32) & 0xFFFFFFFF;

        start[id].limit_low = (sizeof(processor::gdt::tss) & 0xFFFF) - 1;
        start[id].limit_high = sizeof(processor::gdt::tss) >> 16;

        start[id].accessed = 1;
        start[id].code = 1;
        start[id].present = 1;
        start[id].dpl = 3;

        auto stacks = memory::vm::allocate_address_range(6 * 4096);
        memory::vm::map(stacks + 4096);
        memory::vm::map(stacks + 3 * 4096);
        memory::vm::map(stacks + 5 * 4096);

        tss->ist1 = static_cast<uint64_t>(stacks + 2 * 4096);
        tss->ist2 = static_cast<uint64_t>(stacks + 4 * 4096);
        tss->ist3 = static_cast<uint64_t>(stacks + 6 * 4096);

        tss->rsp0 = static_cast<uint64_t>(memory::vm::allocate_address_range(2 * 4096) + 2 * 4096);
        memory::vm::map(virt_addr_t{ tss->rsp0 } - 4096);

        if (processor::ready())
        {
            processor::get_current_core()->kernel_stack = tss->rsp0;
        }
    }
}

extern "C" void load_gdt(processor::gdt::gdtr *);

void processor::gdt::initialize()
{
    memory::zero(_gdt, 7);

    _gdtr.address = _gdt;
    _gdtr.limit = sizeof(gdt_entry) * 7 - 1;

    _setup_gdte(1, true, false);                                // 0x8 kernel code
    _setup_gdte(2, false, false);                               // 0x10 kernel data
    _setup_gdte(3, true, true);                                 // 0x18 userspace code
    _setup_gdte(4, false, true);                                // 0x20 userspace data
    _setup_tss(5);

    load_gdt(&_gdtr);
}

void processor::gdt::ap_initialize()
{
    auto core = processor::get_core(processor::id());
    auto gdt = core->_gdt;
    auto & gdtr = core->_gdtr;
    auto & tss = core->_tss;

    if (processor::id() == processor::bsp())
    {
        tss.rsp0 = _tss.rsp0;
        gdtr.address = _gdt;
        gdtr.limit = sizeof(gdt_entry) * 7 - 1;

        return;
    }

    memory::zero(gdt, 7);

    gdtr.address = gdt;
    gdtr.limit = sizeof(gdt_entry) * 7 - 1;

    _setup_gdte(1, true, false, gdt);
    _setup_gdte(2, false, false, gdt);
    _setup_gdte(3, true, true, gdt);
    _setup_gdte(4, false, true, gdt);
    _setup_tss(5, gdt, &tss);

    load_gdt(&gdtr);
}

/**
 * ReaverOS
 * loader/booter/processor.cpp
 * Processor routines.
 */

/**
 * Reaver Project OS, Rose License
 *
 * Copyright (C) 2011-2012 Reaver Project Team:
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

#include "processor.h"
#include "screen.h"
#include "paging.h"

using Screen::bout;

extern "C"
{
    uint32 _cpu_check_long_mode();
    void _enable_pae_paging();
    void _enable_msr_longmode();
    void _enable_paging(uint32);
    void _setup_gdt();

    void _execute();
}

namespace Processor
{
    PML4 * PagingStructures;
}

void Processor::EnterLongMode()
{
    if (!_cpu_check_long_mode())
    {
        *bout << "PANIC: long mode not supported!";
        for (;;) ;
    }

    PML4 * p = (PML4 *)Memory::PlacePageAligned(sizeof(PML4));
    Memory::Zero((char *)p, sizeof(PML4));

    Processor::PagingStructures = p;

    uint64 i = 0;
    Memory::Map(0, 64 * 1024 * 1024, i);
    
    uint32 cr3 = (uint32)p;
    cr3 &= ~(uint32)0xfff;
    
    _enable_pae_paging();
    _enable_msr_longmode(); 
    _enable_paging(cr3);
    
    Screen::bout->UpdatePagingStructures();

    return;
}

void Processor::SetupGDT()
{
    _setup_gdt();
}

void Processor::Execute(uint32, uint64, uint64, uint32, uint64, uint64)
{
    __asm("jmp _execute");
}

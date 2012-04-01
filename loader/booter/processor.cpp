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

    // identity map first 64 MiB

    PML4 * p = (PML4 *)Memory::PlacePageAligned(sizeof(PML4));
    Memory::Zero((char *)p, sizeof(PML4));

    PageDirectoryPointerTable * pdpt = (PageDirectoryPointerTable *)Memory::PlacePageAligned(sizeof(PageDirectoryPointerTable));
    Memory::Zero((char *)pdpt, sizeof(PageDirectoryPointerTable));
    p->PointerTables[0] = pdpt;
    p->Entries[0].Present = 1;
    p->Entries[0].ReadWrite = 1;
    p->Entries[0].PDPTAddress = ((uint64)pdpt) >> 12;

    PageDirectory * pd = (PageDirectory *)Memory::PlacePageAligned(sizeof(PageDirectory));
    Memory::Zero((char *)pd, sizeof(PageDirectory));
    pdpt->PageDirectories[0] = pd;
    pdpt->Entries[0].Present = 1;
    pdpt->Entries[0].ReadWrite = 1;
    pdpt->Entries[0].PageDirectoryAddress = ((uint64)pd) >> 12;
    
    for (uint32 i = 0; i < 16; i++)
    {
        PageTable * pt = (PageTable *)Memory::PlacePageAligned(sizeof(PageTable));
        Memory::Zero((char *)pt, sizeof(PageTable));
        pd->PageTables[i] = pt;
        pd->Entries[i].Present = 1;
        pd->Entries[i].ReadWrite = 1;
        pd->Entries[i].PageTableAddress = ((uint64)pt) >> 12;

        for (uint32 j = 0; j < 512; j++)
        {
            pt->Entries[j].Present = 1;
            pt->Entries[j].ReadWrite = 1;
            pt->Entries[j].PageAddress = (i * 2 * 1024 * 1024 + j * 4 * 1024) >> 12;
        }
    }

    Processor::PagingStructures = p;
    
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
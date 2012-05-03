/**
 * ReaverOS
 * kernel/memory/pagestack.cpp
 * Page stack implementation.
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

#include "pagestack.h"
#include "vmm.h"
#include "memory.h"

Memory::PageStack::PageStack(Memory::MemoryMap * pMemoryMap)
{
    this->m_iSize = 0;

    Memory::AlignPlacementToPage();
    this->m_pStack = (uint64 *)Memory::pPlacementAddress;
    this->m_pLastPage = (uint64)Memory::pPlacementAddress;
    
    for (uint64 i = 0; i < pMemoryMap->GetNumberOfEntries(); i++)
    {
        if (pMemoryMap->GetEntries()[i].Type() != 1)
        {
            continue;
        }

        uint64 start = pMemoryMap->GetEntries()[i].Base();
        uint64 end = pMemoryMap->GetEntries()[i].End();

        while (start != end)
        {
            this->m_pStack[this->m_iSize] = start;
            this->m_iSize++;

            start += 4096;

            if (this->m_iSize % 512 == 0)
            {
                this->m_pLastPage += 4096;
            }
        }
    }

    // give the stack one additional page for possibly freed at some point pages
    // not much more will be placed, anyway
    Memory::pPlacementAddress = (void *)(this->m_pLastPage + 4 * 1024);
}

Memory::PageStack::~PageStack()
{
}

uint64 Memory::PageStack::Count()
{
    return this->m_iSize;
}

uint64 Memory::PageStack::Pop()
{
    this->m_iSize--;
    uint64 ret = this->m_pStack[this->m_iSize];

    if ((this->m_iSize + 64) % 512 == 0)
    {
        Memory::VMM::UnmapPage(this->m_pLastPage);
    }

    return ret;
}

void Memory::PageStack::Push(uint64 p)
{
    this->m_pStack[this->m_iSize] = p;
    this->m_iSize++;

    if ((this->m_iSize + 32) % 512 == 0)
    {
        Memory::VMM::MapPage(this->m_pLastPage + 4096);
        this->m_pLastPage += 4096;
    }

    return;
}
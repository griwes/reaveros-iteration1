/**
 * ReaverOS
 * kernel/lib/stack.cpp
 * Stack implementation.
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

#include "stack.h"
#include "../memory/vmm.h"
#include "../memory/memory.h"

Lib::Stack::Stack(Memory::MemoryMap * pMemoryMap, uint64 base)
{
    this->m_iSize = 0;

    this->m_pStack = (uint64 *)base;
    this->m_iLastPage = 0;

    for (uint64 i = 0; i < pMemoryMap->GetNumberOfEntries(); i++)
    {
        if (pMemoryMap->GetEntries()[i].Type() != 1)
        {
            continue;
        }

        if (pMemoryMap->GetEntries()[i].End() <= 1024 * 1024)
        {
            continue;
        }

        uint64 start = pMemoryMap->GetEntries()[i].Base();
        uint64 end = pMemoryMap->GetEntries()[i].End();

        while (start < end)
        {
            if (m_iLastPage == 0)
            {
                Memory::VMM::MapPage((uint64)m_pStack, start);
                m_iLastPage = (uint64)m_pStack;
                start += 4096;
                dbg;
                continue;
            }

            this->m_pStack[this->m_iSize] = start;
            this->m_iSize++;

            start += 4096;

            if (this->m_iSize % 512 == 0)
            {
                this->m_iLastPage += 4096;
                Memory::VMM::MapPage(m_iLastPage, Pop());
            }
        }
    }
}

Lib::Stack::~Stack()
{
}

uint64 Lib::Stack::Count()
{
    return this->m_iSize;
}

uint64 Lib::Stack::Pop()
{
    this->m_iSize--;
    uint64 ret = this->m_pStack[this->m_iSize];

    if ((this->m_iSize + 64) % 512 == 0)
    {
        Memory::VMM::UnmapPage(this->m_iLastPage);
    }

    return ret;
}

void Lib::Stack::Push(uint64 p)
{
    this->m_pStack[this->m_iSize] = p;
    this->m_iSize++;

    if ((this->m_iSize + 32) % 512 == 0)
    {
        Memory::VMM::MapPage(this->m_iLastPage + 4096);
        this->m_iLastPage += 4096;
    }

    return;
}

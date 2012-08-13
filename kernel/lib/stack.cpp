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
#include "../memory/vm.h"
#include "../memory/vmm.h"
#include "../memory/memory.h"

Lib::Stack::Stack(Memory::MemoryMap * pMemoryMap, uint64 base)
{
    m_iSize = 0;

    m_pStack = (uint64 *)base;
    m_iLastPage = 0;

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
                continue;
            }

            m_pStack[m_iSize] = start;
            m_iSize++;

            start += 4096;

            if (((m_iLastPage + 4096 - (uint64)m_pStack) / 8) - m_iSize < 16)
            {
                m_iLastPage += 4096;
                Memory::VMM::MapPage(m_iLastPage, m_pStack[--m_iSize]);
            }
        }
    }
}

Lib::Stack::Stack(uint64 start, uint64 end, uint64 base)
{
    m_iSize = 0;
    m_pStack = (uint64 *)base;
    m_iLastPage = 0;
    
    for (; start != end; start++)
    {
        if (m_iLastPage == 0)
        {
            Memory::VMM::MapPage(base);
            m_iLastPage = base;
        }

        m_pStack[m_iSize++] = start;

        if (((m_iLastPage + 4096 - base) / 8) - m_iSize < 16)
        {
            m_iLastPage += 4096;
            Memory::VMM::MapPage(m_iLastPage);
        }
    }
}

Lib::Stack::~Stack()
{
    for (uint64 i = (uint64)m_pStack; i <= m_iLastPage; i += 4096)
    {
        Memory::VMM::UnmapPage(i);
    }
}

uint64 Lib::Stack::Count()
{
    return m_iSize;
}

uint64 Lib::Stack::Pop()
{
    uint64 r = PopSpecial();
    
    if (((m_iLastPage + 4096 - (uint64)m_pStack) / 8) - m_iSize > 4096 / 8 + 64 && Memory::VMM::Ready)
    {
        Memory::VMM::UnmapPage(m_iLastPage);
        m_iLastPage -= 4096;
    }
    
    return r;
}

uint64 Lib::Stack::PopSpecial()
{
    m_iSize--;
    
    return m_pStack[m_iSize];
}

void Lib::Stack::Push(uint64 p)
{
    PushSpecial(p);
    
    if (((m_iLastPage + 4096 - (uint64)m_pStack) / 8) - m_iSize < 16 && Memory::VMM::Ready)
    {
        m_iLastPage += 4096;
        Memory::VMM::MapPage(m_iLastPage);
    }
}

void Lib::Stack::PushSpecial(uint64 p)
{
    m_pStack[m_iSize++] = p;    
}

void Lib::Stack::RegisterPages()
{
    auto reg = Memory::CurrentVAS->GetRegion((uint64)m_pStack);
    
    for (uint64 s = (uint64)m_pStack; s <= m_iLastPage; s += 4096)
    {
        Memory::VM::Page * p = new Memory::VM::Page;
        p->Allocated = 1;
        p->VirtualAddress = s;
        p->PhysicalAddress = Memory::CurrentVAS->m_pPML4->GetPhysicalAddress(s);

        reg->AddPage(p);
    }
}
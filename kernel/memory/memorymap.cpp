/**
 * ReaverOS
 * kernel/memory/memorymap.cpp
 * Memory map implementation.
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

#include "memorymap.h"
#include "memory.h"
#include "../screen/screen.h"

using Screen::kout;
using Screen::nl;

String Memory::MemoryMapEntry::TypeDescription()
{
    const char * description;
        
    switch (this->m_iType)
    {
        case 1:
            description = "Free memory";
            break;
        case 2:
            description = "Reserved memory";
            break;
        case 3:
            description = "ACPI reclaimable memory";
            break;
        case 4:
            description = "ACPI NVS memory";
            break;
        case 5:
            description = "Bad memory";
            break;
        case 0xffff:
            description = "Kernel memory";
            break;
        default:
            description = "Unknown type of memory";
    }
    
    return String::MakeConst(description);
}

Memory::MemoryMapEntry * Memory::MemoryMap::GetEntries()
{
    return this->m_pEntries;
}

uint64 Memory::MemoryMap::CountUsableMemory()
{
    uint64 iSize = 0;

    for (uint32 i = 0; i < this->m_iSize; i++)
    {
        if (this->m_pEntries[i].Type() == 1)
        {
            iSize += this->m_pEntries[i].Length();
        }
    }

    return iSize;
}

Memory::MemoryMap::MemoryMap(Memory::MemoryMapEntry * pMemMap, uint32 iMemoryMapSize)
{
    uint64 iKernelEntryIndex = 0;
    uint64 iAdditionalBase = 0;
    uint64 iAdditionalLength = 0;
    uint32 iAdditionalType = 0;

    while (pMemMap[iKernelEntryIndex].Type() != 0xffff)
    {
        iKernelEntryIndex++;
    }

    Memory::MemoryMapEntry * pKernelEntry = pMemMap + iKernelEntryIndex;
    
    for (uint32 i = 0; i < iMemoryMapSize; i++)
    {
        if (pMemMap[i].Type() == 0xffff)
        {
            continue;
        }

        if (pMemMap[i].Base() >= pKernelEntry->Base() && pMemMap[i].Base() < pKernelEntry->End())
        {
            if (pMemMap[i].End() <= pKernelEntry->End())
            {
                Memory::Zero(&pMemMap[i]);
            }

            else
            {
                pMemMap[i].Base() = pKernelEntry->End();
            }
        }

        else if (pMemMap[i].End() > pKernelEntry->Base() && pMemMap[i].End() < pKernelEntry->End())
        {
            pMemMap[i].Length() = pKernelEntry->Base() - pMemMap[i].Base();
        }

        else if (pMemMap[i].Base() < pKernelEntry->Base() && pMemMap[i].End() > pKernelEntry->End())
        {
            iAdditionalBase = pKernelEntry->End();
            iAdditionalLength = pMemMap[i].End() - iAdditionalBase;
            iAdditionalType = pMemMap[i].Type();
        }
    }

    this->m_pEntries = new MemoryMapEntry[iMemoryMapSize + (iAdditionalLength != 0 ? 1 : 0)];
    this->m_iSize = iMemoryMapSize + (iAdditionalLength != 0 ? 1 : 0);

    uint64 iLowestBase = 0xFFFFFFFFFFFFFFFF;
    uint64 iLowestIndex = 0;
    for (uint32 i = 0, j = 0; i < iMemoryMapSize; i++, j++)
    {
        iLowestBase = 0xFFFFFFFFFFFFFFFF;
        iLowestIndex = 0;
        
        for (uint32 k = 0; k < iMemoryMapSize; k++)
        {
            if (pMemMap[k].Base() < iLowestBase && pMemMap[k].Type() != 0)
            {
                iLowestBase = pMemMap[k].Base();
                iLowestIndex = k;
            }
        }

        if (iAdditionalBase < iLowestBase)
        {
            this->m_pEntries[j].Type() = iAdditionalType;
            this->m_pEntries[j].Base() = iAdditionalBase;
            this->m_pEntries[j].Length() = iAdditionalLength;

            j++;
        }

        this->m_pEntries[j] = pMemMap[iLowestIndex];

        pMemMap[iLowestIndex].Type() = 0;
    }
}

void Memory::MemoryMap::PrintMemoryMap()
{
    Screen::Console::Mode m = kout->GetMode();
    kout->Hex(16, true);

    *kout << "Base address       | Length             | Type" << nl;
    *kout << "-------------------|--------------------|----------------------------" << nl;

    for (uint32 i = 0; i < this->m_iSize; i++)
    {
        *kout << Memory::pMemoryMap->GetEntries()[i].Base();
        *kout << " | ";
        *kout << Memory::pMemoryMap->GetEntries()[i].Length();
        *kout << " | ";
        *kout << Memory::pMemoryMap->GetEntries()[i].TypeDescription();
        *kout << nl;
    }

    *kout << "-------------------|--------------------|----------------------------" << nl;
    
    uint64 total = Memory::pMemoryMap->CountUsableMemory();
    uint64 gbs = total >> 30;
    uint64 mbs = (total >> 20) & 1023;
    uint64 kbs = (total >> 10) & 1023;

    kout->Dec();
    *kout << "Total usable memory: " << gbs << " GiB " << mbs << " MiB " << kbs << " KiB" << nl << nl;
    
    kout->SetMode(m);
}

uint32 Memory::MemoryMap::GetNumberOfEntries()
{
    return this->m_iSize;
}

uint32 Memory::MemoryMap::GetMemoryType(uint64 addr)
{
    for (uint32 i = 0; i < this->m_iSize; i++)
    {
        if (this->m_pEntries[i].Base() <= addr && this->m_pEntries[i].End() > addr)
        {
            return this->m_pEntries[i].Type();
        }
    }

    return 0xdeadc0de;
}

String Memory::MemoryMap::GetMemoryTypeDescription(uint64 addr)
{
    for (uint32 i = 0; i < this->m_iSize; i++)
    {
        if (this->m_pEntries[i].Base() <= addr && this->m_pEntries[i].End() > addr)
        {
            return this->m_pEntries[i].TypeDescription();
        }
    }
    
    return String::MakeConst("Address not available");
}
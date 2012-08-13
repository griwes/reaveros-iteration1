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
#include "../screen/console.h"

using Screen::kout;
using Screen::nl;

Lib::String Memory::MemoryMapEntry::TypeDescription()
{
    const char * description;
        
    switch (m_iType)
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
    
    return Lib::String(description);
}

Memory::MemoryMapEntry * Memory::MemoryMap::GetEntries()
{
    return m_pEntries;
}

uint64 Memory::MemoryMap::CountUsableMemory()
{
    uint64 iSize = 0;

    for (uint32 i = 0; i < m_iSize; i++)
    {
        if (m_pEntries[i].Type() == 1)
        {
            iSize += m_pEntries[i].Length();
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
            pMemMap[i].Length() = pKernelEntry->Base() - pMemMap[i].Base();
        }
    }

    m_pEntries = new MemoryMapEntry[iMemoryMapSize + (iAdditionalLength != 0 ? 1 : 0)];
    m_iSize = iMemoryMapSize + (iAdditionalLength != 0 ? 1 : 0);

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
            m_pEntries[j].Type() = iAdditionalType;
            m_pEntries[j].Base() = iAdditionalBase;
            m_pEntries[j].Length() = iAdditionalLength;

            j++;
        }

        m_pEntries[j] = pMemMap[iLowestIndex];

        pMemMap[iLowestIndex].Type() = 0;
    }
}

void Memory::MemoryMap::PrintMemoryMap()
{
    uint64 m = kout->GetBase();
    kout->HexNumbers(16);

    *kout << "Base address       | Length             | Type" << nl;
    *kout << "-------------------|--------------------|----------------------------" << nl;

    for (uint32 i = 0; i < m_iSize; i++)
    {
        *kout << Memory::SystemMemoryMap->GetEntries()[i].Base();
        *kout << " | ";
        *kout << Memory::SystemMemoryMap->GetEntries()[i].Length();
        *kout << " | ";
        *kout << Memory::SystemMemoryMap->GetEntries()[i].TypeDescription();
        *kout << nl;
    }

    *kout << "-------------------|--------------------|----------------------------" << nl;
    
    uint64 total = Memory::SystemMemoryMap->CountUsableMemory();
    uint64 gibs = total >> 30;
    uint64 mibs = (total >> 20) & 1023;
    uint64 kibs = (total >> 10) & 1023;

    kout->SetBase(10);
    *kout << "Total usable memory: " << gibs << " GiB " << mibs << " MiB " << kibs << " KiB" << nl << nl;
    kout->SetBase(m);
}

uint32 Memory::MemoryMap::GetNumberOfEntries()
{
    return m_iSize;
}

uint32 Memory::MemoryMap::GetMemoryType(uint64 addr)
{
    for (uint32 i = 0; i < m_iSize; i++)
    {
        if (m_pEntries[i].Base() <= addr && m_pEntries[i].End() > addr)
        {
            return m_pEntries[i].Type();
        }
    }

    return 0xdeadc0de;
}

Lib::String Memory::MemoryMap::GetMemoryTypeDescription(uint64 addr)
{
    for (uint32 i = 0; i < m_iSize; i++)
    {
        if (m_pEntries[i].Base() <= addr && m_pEntries[i].End() > addr)
        {
            return m_pEntries[i].TypeDescription();
        }
    }
    
    return Lib::String("Address not available");
}
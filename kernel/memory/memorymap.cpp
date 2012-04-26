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

    for (int32 i = 0; i < this->m_iSize; i++)
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
    uint32 iKernelEntryIndex = 0;
    uint32 iAdditional = 0;
    
    while (pMemMap[iKernelEntryIndex].Type() != 0xffff)
    {
        iKernelEntryIndex++;
    }
    
    Memory::MemoryMapEntry * pKernelEntry = pMemMap + iKernelEntryIndex;
    
    for (uint32 i = 0; i < iMemoryMapSize; i++, pMemMap++)
    {
        if (pMemMap->Base() >= pKernelEntry->Base() && pMemMap->Base() < pKernelEntry->End())
        {
            if (pMemMap->End() <= pKernelEntry->End())
            {
                Memory::Zero(pMemMap);
            }

            else
            {
                pMemMap->Base() = pKernelEntry->End();
            }
        }

        else if (pMemMap->End() > pKernelEntry->Base() && pMemMap->End() < pKernelEntry->End())
        {
            pMemMap->Length() = pKernelEntry->Base() - pMemMap->Base() - 1;
        }

        else if (pMemMap->Base() < pKernelEntry->Base() && pMemMap->End() > pKernelEntry->End())
        {
            iAdditional++;
        }
    }

    this->m_pEntries = new MemoryMapEntry[iMemoryMapSize + iAdditional];
    this->m_iSize = iMemoryMapSize + iAdditional;
}
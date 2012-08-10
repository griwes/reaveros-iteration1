/**
 * ReaverOS
 * kernel/memory/vmm.cpp
 * Virtual memory manager implementation.
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

#include "vmm.h"

namespace Memory
{
    namespace VMM
    {
        bool Ready = false;
    }
}

// few things are known here:
// 1. if there are no paging structures for given area in PagingStructuresPool, then it's being used for the first time
//     (always true, as PSPool PSs are never freed and on the start, they are sorted)
// 2. when pool index is put back on stack, it has already created PSs out there 
//     (always true, as when id is given back, it must have been already used and mapped)
// when you connect those, you must never worry about recursive AllocPagingPages call's ArePSAvailable call returns 
// something else than 3
void * Memory::VMM::AllocPagingPages()
{   
    if (VMM::Ready)
    {
        if (PagingStructures)
        {               
            auto PSPool = /*(CorePagingStructures ? CorePagingStructures : */PagingStructures;//);

            void * pgs = (void *)(VM::PagingStructuresPoolBase + PSPool->PopSpecial() * 2 * 4096);
            
            uint64 nextidx = PSPool->PopSpecial();
            uint64 next = nextidx * 2 * 4096;
            next += VM::PagingStructuresPoolBase;
            
            switch (CurrentVAS->m_pPML4->ArePSAvailable(next))
            {
                case 0:
                case 1:
                case 2:
                    break;
                case 3:
                    VMM::MapPage(next);
            }
            
            PSPool->PushSpecial(nextidx);
        }
        
        else
        {
            PANIC("VMM::Ready = true, but PagingStructures = nullptr!");
        }
    }

    else
    {
        PANIC("This panic is so ridiculous I don't even want to fill it.");
    }
}

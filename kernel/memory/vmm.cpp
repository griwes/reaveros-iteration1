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

void * Memory::VMM::AllocPagingPages()
{   
    if (VMM::Ready)
    {
        if (PagingStructures)
        {               
            auto PSPool = /*(CorePagingStructures ? CorePagingStructures : */PagingStructures;//);

            void * pgs = (void *)(VM::PagingStructuresPoolBase + PSPool->Pop() * 2 * 4096);
            
            uint64 nextidx = PSPool->Pop();
            uint64 next = nextidx * 2 * 4096;
            next += VM::PagingStructuresPoolBase;
            
            uint64 counter = 0;
            
            switch (CurrentVAS->m_pPML4->ArePSAvailable(next))
            {
                case 0:
                    CurrentVAS->m_pPML4->InjectPS(next, PSPool->Pop());
                    counter++;
                case 1:
                    CurrentVAS->m_pPML4->InjectPS(next, PSPool->Pop());
                    counter++;
                case 2:
                {
                    CurrentVAS->m_pPML4->InjectPS(next, PSPool->Pop());
                    counter++;
                    
                    uint64 indices[3] = {0};
                    
                    for (uint64 i = 0; i < counter; i++)
                    {
                        indices[i] = PSPool->Pop();
                        VMM::MapPage(VM::PagingStructuresPoolBase + indices[i] * 2 * 4096);
                    }
                    
                    for (uint64 i = counter; i > 0; i--)
                    {
                        PSPool->PushSpecial(i - 1);
                    }
                }
                case 3:
                    VMM::MapPage(next);
            }
            
            PSPool->PushSpecial(nextidx);
            
            return pgs;
        }
        
        else
        {
            PANIC("VMM::Ready = true, but PagingStructures = nullptr!");
            
            return nullptr;
        }
    }

    else
    {
        PANIC("This panic is so ridiculous I don't even want to fill it.");
        
        return nullptr;
    }
}

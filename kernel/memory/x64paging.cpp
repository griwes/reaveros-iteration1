/**
 * Reaver Project OS, Rose License
 * 
 * Copyright (C) 2011-2013 Reaver Project Team:
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

#include <memory/x64paging.h>
#include <memory/memory.h>
#include <memory/pmm.h>
#include <processor/processor.h>

namespace
{
    class address_generator
    {
    public:
        address_generator(uint64_t id) : _id(id)
        {
            if (_id < 256)
            {
                 PANIC("Tried to create address generator for lower half PML4 recursive entry");
            }
        }
        
        memory::x64::pml4 * pml4()
        {
            return (memory::x64::pml4 *)pt(_id, _id, _id);
        }
        
        memory::x64::pdpt * pdpt(uint64_t pml4e)
        {
            return (memory::x64::pdpt *)pt(_id, _id, pml4e);
        }
        
        memory::x64::page_directory * pd(uint64_t pml4e, uint64_t pdpte)
        {
            return (memory::x64::page_directory *)pt(_id, pml4e, pdpte);
        }
        
        memory::x64::page_table * pt(uint64_t pml4e, uint64_t pdpte, uint64_t pde)
        {
            return (memory::x64::page_table *)(0xFFFF000000000000 + (_id << 39) + (pml4e << 30) + (pdpte << 21) + (pde << 12));
        }
        
    private:
        uint64_t _id;
    };
}

void memory::x64::map(uint64_t virtual_start, uint64_t virtual_end, uint64_t physical_start, bool foreign)
{
    address_generator gen(foreign ? 257 : 256);
    
    if (virtual_start >= 0xFFFF800000000000 && virtual_start < 0xFFFF800000000000 + 2 * 512 * 1024 * 1024 * 1024)
    {
        PANIC("Trying to map something in paging structs area");
    }
    
    virtual_start &= ~(uint64_t)4095;
    virtual_end += 4095;
    virtual_end &= ~(uint64_t)4095;
    
    if (virtual_end <= virtual_start)
    {
        return;
    }
    
    uint64_t startpml4e = (virtual_start >> 39) & 511;
    uint64_t startpdpte = (virtual_start >> 30) & 511;
    uint64_t startpde = (virtual_start >> 21) & 511;
    uint64_t startpte = (virtual_start >> 12) & 511;
    
    uint64_t endpml4e = (virtual_end >> 39) & 511;
    uint64_t endpdpte = (virtual_end >> 30) & 511;
    uint64_t endpde = (virtual_end >> 21) & 511;
    uint64_t endpte = (virtual_end >> 12) & 511;
    
    while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte))
    {
        if (!gen.pml4()->entries[startpml4e].present)
        {
            gen.pml4()->entries[startpml4e] = memory::pmm::pop();
        }
        
        pdpt * table = gen.pdpt(startpml4e);
        processor::invlpg((uint64_t)table);
        
        while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
            && startpdpte < 512)
        {
            if (!(*table)[startpdpte].present)
            {
                (*table)[startpdpte] = memory::pmm::pop();
            }
            
            page_directory * pd = gen.pd(startpml4e, startpdpte);
            processor::invlpg((uint64_t)pd);
            
            while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
                && startpde < 512)
            {
                if (!(*pd)[startpde].present)
                {
                    (*pd)[startpde] = memory::pmm::pop();
                }
                
                page_table * pt = gen.pt(startpml4e, startpdpte, startpde);
                processor::invlpg((uint64_t)pt);
                
                while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
                    && startpte < 512)
                {
                    if ((*pt)[startpte].present && physical_start != (*pt)[startpte].address << 12)
                    {
                        PANIC("Tried to map something at already mapped page"); // TODO: fix PANIC to provide more debugging info
                    }
                    
                    (*pt)[startpte++] = physical_start;
                    processor::invlpg(virtual_start);
                    
                    physical_start += 4096;
                    virtual_start += 4096;
                }
                
                if (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte))
                {
                    startpde++;
                    startpte = 0;
                }
                
                else
                {
                    return;
                }
            }
            
            if (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte))
            {
                startpde = 0;
                startpdpte++;
            }
            
            else
            {
                return;
            }
        }
        
        if (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte))
        {
            startpdpte = 0;
            startpml4e++;
        }
        
        else
        {
            return;
        }
    }
}

uint64_t memory::x64::get_physical_address(uint64_t addr, bool foreign)
{
    address_generator gen(foreign ? 257 : 256);
    
    if (gen.pml4()->entries[(addr >> 39) & 511].present)
    {
        if (gen.pdpt((addr >> 39) & 511)->entries[(addr >> 30) & 511].present)
        {
            if (gen.pd((addr >> 39) & 511, (addr >> 30) & 511)->entries[(addr >> 21) & 511].present)
            {
                if (gen.pt((addr >> 39) & 511, (addr >> 30) & 511, (addr >> 21) & 511)->entries[(addr >> 12) & 511].present)
                {
                    return gen.pt((addr >> 39) & 511, (addr >> 30) & 511, (addr >> 21) & 511)->entries[(addr >> 12) & 511].address << 12;
                }
            }
        }
    }
    
    PANIC("Tried to get physical address of not mapped page");
    
    return 0;
}
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

#include <memory/x64paging.h>
#include <memory/memory.h>
#include <memory/manager.h>
#include <processor/processor.h>

void memory::x64::pml4::map(uint64_t virtual_start, uint64_t virtual_end, uint64_t physical_start)
{
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
        if (!entries[startpml4e].present)
        {
            memory::default_allocator->align(4096);
            entries[startpml4e] = new pdpt;
        }
        
        pdpt * table = (pdpt *)(entries[startpml4e].address << 12);
        
        while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
            && startpdpte < 512)
        {
            if (!(*table)[startpdpte].present)
            {
                memory::default_allocator->align(4096);
                (*table)[startpdpte] = new page_directory;
            }
            
            page_directory * pd = (page_directory *)((*table)[startpdpte].address << 12);
            
            while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
                && startpde < 512)
            {
                if (!(*pd)[startpde].present)
                {
                    memory::default_allocator->align(4096);
                    (*pd)[startpde] = new page_table;
                }
                
                page_table * pt = (page_table *)((*pd)[startpde].address << 12);
                
                while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
                    && startpte < 512)
                {
                    if ((*pt)[startpte].present && physical_start != (*pt)[startpte].address << 12)
                    {
                        screen::printf("\nAt address 0x%016x:", physical_start);
                        PANIC("tried to map something at already mapped page");
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

void memory::x64::pml4::unmap(uint64_t virtual_start, uint64_t virtual_end)
{
    virtual_start &= ~(uint64_t)4095;
    virtual_end += 4095;
    virtual_end &= ~(uint64_t)4095;
    
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
        if (!entries[startpml4e].present)
        {
            PANIC("tried to unmap not mapped page");
        }
        
        pdpt * table = (pdpt *)(entries[startpml4e].address << 12);
        
        while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
            && startpdpte < 512)
        {
            if (!(*table)[startpdpte].present)
            {
                PANIC("tried to unmap not mapped page");
            }
            
            page_directory * pd = (page_directory *)((*table)[startpdpte].address << 12);
            
            while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
                && startpde < 512)
            {
                if (!(*pd)[startpde].present)
                {
                    PANIC("tried to unmap not mapped page");
                }
                
                page_table * pt = (page_table *)((*pd)[startpde].address << 12);
                
                while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
                    && startpte < 512)
                {
                    if (!(*pt)[startpte].present)
                    {
                        PANIC("tried to unmap not mapped page");
                    }
                    
                    (*pt)[startpte++].present = 0;
                    processor::invlpg(virtual_start);
                    
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


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

/*void memory::x64::pml4::map(uint64_t virtual_start, uint64_t virtual_end, uint64_t physical_start)
{
    virtual_start &= (uint64_t)4096;
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
}*/

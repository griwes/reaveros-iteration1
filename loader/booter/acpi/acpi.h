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

#pragma once

#include <processor/processor.h>
#include <screen/screen.h>

namespace acpi
{
    struct rsdp
    {
        char signature[8];
        uint8_t checksum;
        char oemid[6];
        uint8_t revision;
        uint32_t rsdt_ptr;
        uint32_t length;
        uint64_t xsdt_ptr;
        uint8_t ext_checksum;
        uint8_t reserved[3];
        
        bool validate()
        {
            uint8_t checksum = 0;
            
            for (uint8_t i = 0; i < sizeof(rsdp); ++i)
            {
                checksum += *((uint8_t *)this + i);
            }
            
            return signature[0] == 'R' && signature[1] == 'S' && signature[2] == 'D' && signature[3] == ' '
                && signature[4] == 'P' && signature[5] == 'T' && signature[6] == 'R' && signature[7] == ' '
                && !checksum;
        }
    } __attribute__((packed));
    
    rsdp * find_rsdp();
    
    processor::numa_env * find_numa_domains();
}

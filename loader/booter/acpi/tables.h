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
            if (signature[0] == 'R' && signature[1] == 'S' && signature[2] == 'D' && signature[3] == ' '
                && signature[4] == 'P' && signature[5] == 'T' && signature[6] == 'R' && signature[7] == ' ')
            {
                uint8_t checksum = 0;
            
                for (uint8_t i = 0; i < sizeof(rsdp); ++i)
                {
                    checksum += *((uint8_t *)this + i);
                }
            
                return !checksum;
            }
            
            return false;
        }
    } __attribute__((packed));
    
    struct description_table_header
    {
        char signature[4];
        uint32_t length;
        uint8_t revision;
        uint8_t checksum;
        char oemid[6];
        uint64_t oem_tableid;
        uint32_t oem_revision;
        uint32_t creator_id;
        uint32_t creator_revision;
        
        bool validate(const char * sign)
        {
            if (signature[0] == sign[0] && signature[1] == sign[1] && signature[2] == sign[2] && signature[3] == sign[3])
            {
                uint8_t checksum = 0;
            
                for (uint32_t i = 0; i < length; ++i)
                {
                    checksum += *((uint8_t *)this + i);
                }
                
                return !checksum;
            }
            
            return false;
        }
    } __attribute__((packed));
    
    struct rsdt : public description_table_header
    {
        uint32_t entries[1];
    } __attribute__((packed));
    
    struct xsdt : public description_table_header
    {
        uint64_t entries[1];
    } __attribute__((packed));
    
    struct srat_lapic_entry
    {
        uint8_t domain;
        uint8_t apic_id;
        uint32_t flags;
        uint8_t sapic_eid;
        uint8_t domain2;
        uint16_t domain3;
        uint32_t clock_domain;
    } __attribute__((packed));
    
    struct srat_memory_entry
    {
        uint32_t domain;
        uint16_t reserved;
        uint64_t base;
        uint64_t length;
        uint32_t reserved2;
        uint32_t flags;
        uint64_t reserved3;
    } __attribute__((packed));
    
    struct srat_x2apic_entry
    {
        uint16_t reserved;
        uint32_t domain;
        uint32_t x2apic_id;
        uint32_t flags;
        uint32_t clock_domain;
        uint32_t reserved2;
    } __attribute__((packed));
    
    struct srat_entry
    {
        uint8_t type;
        uint8_t length;
    } __attribute__((packed));
    
    struct srat : public description_table_header
    {
        srat_entry entries[1];
    } __attribute__((packed));
    
    extern rsdt * root;
    extern xsdt * new_root;
}
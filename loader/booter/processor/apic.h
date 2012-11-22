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

#include <acpi/tables.h>

namespace processor
{
    struct ioapic
    {
        ioapic() : id(0), base_address(0), base_int(0), size(0), next(nullptr) {}
        
        uint8_t id;
        uint32_t base_address;
        uint32_t base_int;
        uint8_t size;
        
        ioapic * next;
        
        void write_register(uint8_t reg, uint32_t val)
        {
            *(uint32_t *)(base_address) = reg;
            *(uint32_t *)(base_address + 0x10) = val;
        }
        
        uint32_t read_register(uint8_t reg)
        {
            *(uint32_t *)(base_address) = reg;
            return *(uint32_t *)(base_address + 0x10);
        }
    };
    
    struct lapic
    {
        lapic() : acpi_id(0), apic_id(0), nmi_int(0), nmi_specified(false), domain_specified(false), next(nullptr) {}
        
        uint8_t acpi_id;
        uint8_t apic_id;
        uint8_t nmi_int;
        uint16_t nmi_flags;
        bool nmi_specified;
        bool domain_specified;
        
        lapic * next;
    };
    
    struct x2apic
    {
        x2apic() : apic_id(0), acpi_uuid(0), nmi_int(0), nmi_specified(false), domain_specified(false), next(nullptr) {}
        
        uint32_t apic_id;
        uint32_t acpi_uuid;
        uint8_t nmi_int;
        uint16_t nmi_flags;
        bool nmi_specified;
        bool domain_specified;
        
        x2apic * next;
    };
    
    struct nmi
    {
        uint32_t id;
        nmi * next;
    };
    
    struct apic_env
    {
        apic_env(acpi::madt *);
        
        void add_ioapic(ioapic *);
        void add_lapic(lapic *);
        void add_x2apic(x2apic *);
        void add_global_nmi(uint32_t);
        
        lapic * get_lapic(uint8_t);
        x2apic * get_x2apic(uint32_t);
        
        uint64_t base;
        ioapic * ioapics = nullptr;
        lapic * lapics = nullptr;
        x2apic * x2apics = nullptr;
        nmi * global_nmis = nullptr;
    };
    
    void setup_io_apics(processor::apic_env *);
    void setup_lapic();
}
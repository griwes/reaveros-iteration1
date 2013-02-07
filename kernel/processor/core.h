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

#pragma once

#include <memory/vm.h>

namespace processor
{
    class core
    {
    public:
        core() : _is_valid(false), _is_nmi_valid(false) {}
        
        core(uint32_t apic_id, uint32_t acpi_id, bool is_lapic = true) : _acpi_id(acpi_id), _apic_id(apic_id), 
            _is_local_apic(is_lapic), _is_valid(true), _is_nmi_valid(false)
        {
        }
        
        uint32_t acpi_id()
        {
            return _acpi_id;
        }
        
        uint32_t apic_id()
        {
            return _apic_id;
        }
        
        bool lapic()
        {
            return _is_local_apic;
        }
        
        void set_nmi(uint32_t vector, uint32_t flags)
        {
            _nmi_vector = vector;
            _nmi_flags = flags;
            
            _is_nmi_valid = true;
        }
        
        uint8_t started = 0;
        
    private:
        uint32_t _acpi_id;
        uint32_t _apic_id;
        bool _is_local_apic;
        
        uint32_t _nmi_vector;
        uint32_t _nmi_flags;
        
        bool _is_valid;
        bool _is_nmi_valid;
    };
}
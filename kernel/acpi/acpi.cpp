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

#include <acpi/acpi.h>
#include <acpi/tables.h>
#include <memory/vm.h>
#include <processor/processor.h>
#include <screen/screen.h>

namespace
{    
    acpi::rsdt * root = nullptr;
    acpi::xsdt * new_root = nullptr;
    
    void _install_rsdt(acpi::rsdp * ptr)
    {
        new_root = nullptr;
        
        memory::vm::map_multiple(memory::vm::acpi_temporal_mapping_start, memory::vm::acpi_temporal_mapping_end, ptr->rsdt_ptr);
        
        if (((acpi::rsdt *)(memory::vm::acpi_temporal_mapping_start + ptr->rsdt_ptr % 4096))->validate("RSDT"))
        {
            root = (acpi::rsdt *)(memory::vm::acpi_temporal_mapping_start + ptr->rsdt_ptr % 4096);
            
            return;
        }
        
        else
        {
            PANIC("RSDT invalid");
        }
    }
    
    void _install_xsdt(acpi::rsdp * ptr)
    {
        memory::vm::map_multiple(memory::vm::acpi_temporal_mapping_start, memory::vm::acpi_temporal_mapping_end, ptr->xsdt_ptr);
        
        if (((acpi::xsdt *)(memory::vm::acpi_temporal_mapping_start + ptr->xsdt_ptr % 4096))->validate("XSDT"))
        {
            new_root = (acpi::xsdt *)(memory::vm::acpi_temporal_mapping_start + ptr->xsdt_ptr % 4096);
            
            return;
        }
        
        else
        {
            memory::vm::unmap(memory::vm::acpi_temporal_mapping_start, memory::vm::acpi_temporal_mapping_end, false);
            
            screen::print(tag::acpi, "XSDT invalid, falling back to RSDT");
            
            _install_rsdt(ptr);
        }
    }
    
    void _install_root(acpi::rsdp * ptr)
    {
        if (ptr->revision)
        {
            _install_xsdt(ptr);
        }
        
        else
        {
            _install_rsdt(ptr);
        }
    }
    
    // TODO: external (bootloader) version of this function
    acpi::rsdp * _find_rsdp()
    {
        uint64_t ebda = *(uint16_t *)0x40E << 4;
        
        acpi::rsdp * ptr = (acpi::rsdp *)ebda;
        
        while (ptr < (acpi::rsdp *)(ebda + 1024))
        {        
            if (ptr->validate())
            {            
                _install_root(ptr);
                
                return ptr;
            }
            
            else
            {
                ptr = (acpi::rsdp *)((uint64_t)ptr + 16);
            }
        }
        
        ptr = (acpi::rsdp *)0xe0000;
        
        while (ptr < (acpi::rsdp *)0x100000)
        {
            if (ptr->validate())
            {
                _install_root(ptr);
                
                return ptr;
            }
            
            else
            {
                ptr = (acpi::rsdp *)((uint64_t)ptr + 16);
            }
        }
        
        PANIC("RSDP not found!");
        
        return nullptr;
    }

    acpi::description_table_header * _find_table(const char * sign, uint64_t addr)
    {
        acpi::description_table_header * table;
        
        if (new_root)
        {
            for (uint64_t i = 0; i < (new_root->length - 36) / 8; ++i)
            {
                table = (acpi::description_table_header *)(addr + new_root->entries[i] % 4096);
                
                memory::vm::map_multiple(addr, addr + 16 * 1024, new_root->entries[i]);
                
                if (table->validate(sign))
                {
                    return table;
                }
                
                memory::vm::unmap(addr, addr + 16 * 1024, false);
            }
        }
        
        else
        {
            for (uint64_t i = 0; i < (root->length - 36) / 4; ++i)
            {
                table = (acpi::description_table_header *)(addr + root->entries[i] % 4096);
                
                memory::vm::map_multiple(addr, addr + 16 * 1024, root->entries[i]);
                
                if (table->validate(sign))
                {
                    return table;
                }
                
                memory::vm::unmap(addr, addr + 16 * 1024);
            }
        }
        
        return nullptr;
    }
}

void acpi::initialize(processor::core * , uint64_t & , processor::ioapic * , uint64_t & )
{
    _find_rsdp();
}

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
#include <processor/core.h>
#include <processor/ioapic.h>

namespace
{    
    acpi::rsdt * root = nullptr;
    acpi::xsdt * new_root = nullptr;
    
    void _install_rsdt(acpi::rsdp * ptr)
    {
        new_root = nullptr;
        
        memory::vm::map_multiple(memory::vm::acpi_temporal_rsdt_mapping_start, memory::vm::acpi_temporal_rsdt_mapping_end, 
            ptr->rsdt_ptr);
        
        if (((acpi::rsdt *)(memory::vm::acpi_temporal_rsdt_mapping_start + ptr->rsdt_ptr % 4096))->validate("RSDT"))
        {
            root = (acpi::rsdt *)(memory::vm::acpi_temporal_rsdt_mapping_start + ptr->rsdt_ptr % 4096);
            
            return;
        }
        
        else
        {
            PANIC("RSDT invalid");
        }
    }
    
    void _install_xsdt(acpi::rsdp * ptr)
    {
        memory::vm::map_multiple(memory::vm::acpi_temporal_rsdt_mapping_start, memory::vm::acpi_temporal_rsdt_mapping_end, 
            ptr->xsdt_ptr);
        
        if (((acpi::xsdt *)(memory::vm::acpi_temporal_rsdt_mapping_start + ptr->xsdt_ptr % 4096))->validate("XSDT"))
        {
            new_root = (acpi::xsdt *)(memory::vm::acpi_temporal_rsdt_mapping_start + ptr->xsdt_ptr % 4096);
            
            return;
        }
        
        else
        {
            memory::vm::unmap(memory::vm::acpi_temporal_rsdt_mapping_start, memory::vm::acpi_temporal_rsdt_mapping_end, false);
            
            screen::print(tag::acpi, "XSDT invalid, falling back to RSDT\n");
            
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

    acpi::description_table_header * _find_table(const char * sign)
    {
        acpi::description_table_header * table;
        
        if (new_root)
        {
            for (uint64_t i = 0; i < (new_root->length - 36) / 8; ++i)
            {
                table = (acpi::description_table_header *)(memory::vm::acpi_temporal_table_mapping_start + 
                    new_root->entries[i] % 4096);
                
                memory::vm::map_multiple(memory::vm::acpi_temporal_table_mapping_start, memory::vm::acpi_temporal_table_mapping_end, 
                    new_root->entries[i]);
                
                if (table->validate(sign))
                {
                    return table;
                }
                
                memory::vm::unmap(memory::vm::acpi_temporal_table_mapping_start, memory::vm::acpi_temporal_table_mapping_end, false);
            }
        }
        
        else
        {
            for (uint64_t i = 0; i < (root->length - 36) / 4; ++i)
            {
                table = (acpi::description_table_header *)(memory::vm::acpi_temporal_table_mapping_start + root->entries[i] % 4096);
                
                memory::vm::map_multiple(memory::vm::acpi_temporal_table_mapping_start, memory::vm::acpi_temporal_table_mapping_end, 
                    root->entries[i]);
                
                if (table->validate(sign))
                {
                    return table;
                }
                
                memory::vm::unmap(memory::vm::acpi_temporal_table_mapping_start, memory::vm::acpi_temporal_table_mapping_end);
            }
        }
        
        return nullptr;
    }
}

void acpi::initialize(processor::core * cores, uint64_t & core_num, processor::ioapic * ioapics, uint64_t & ioapic_num)
{
    _find_rsdp();
    
    madt * table = (madt *)_find_table("MADT");
    
    if (!table)
    {        
        return;
    }
    
    uint64_t lic_address = table->lic_address;
    
    madt_entry * entry = table->entries;
    
    while ((uint64_t)entry - (uint64_t)table < table->length)
    {
        switch (entry->type)
        {
            case 0:
            {
                auto lapic = (acpi::madt_lapic_entry *)((uint64_t)entry + sizeof(*entry));
                
                if (lapic->flags & 1)
                {
                    new ((void *)(cores + core_num++)) processor::core(lapic->apic_id, lapic->acpi_id);
                }
                
                break;
            }
            
            case 1:
            {
                auto ioapic = (acpi::madt_ioapic_entry *)((uint64_t)entry + sizeof(*entry));
                
                new ((void *)(ioapics + ioapic_num++)) processor::ioapic(ioapic->apic_id, ioapic->base_int, ioapic->base_address);
                                
                break;
            }
            
            case 3:
            {
                auto nmi = (acpi::madt_nmi_source_entry *)((uint64_t)entry + sizeof(*entry));
                
                for (uint64_t i = 0; i < ioapic_num; ++i)
                {
                    if (ioapics[i].set_global_nmi(nmi->int_number, nmi->flags))
                    {
                        break;
                    }
                }
                
                break;
            }
            
            case 5:
            {
                auto override = (acpi::madt_lapic_address_override_entry *)((uint64_t)entry + sizeof(*entry));
                
                lic_address = override->base_address;
                
                break;
            }
            
            case 9:
            {
                auto x2apic = (acpi::madt_x2apic_entry *)((uint64_t)entry + sizeof(*entry));
                
                if (x2apic->flags & 1)
                {
                    new ((void *)(cores + core_num++)) processor::core(x2apic->x2apic_id, x2apic->acpi_uuid, false);
                }
                
                break;
            }
        }
        
        entry = (acpi::madt_entry *)((uint64_t)entry + entry->length);
    }
    
    entry = table->entries;
    
    while ((uint64_t)entry - (uint64_t)table < table->length)
    {
        switch (entry->type)
        {
            case 4:
            {
                auto lapic_nmi = (acpi::madt_lapic_nmi_entry *)((uint64_t)entry + sizeof(*entry));
                
                if (lapic_nmi->acpi_id == 0xff)
                {                    
                    for (uint64_t i = 0; i < core_num; ++i)
                    {
                        if (cores[i].lapic())
                        {
                            cores[i].set_nmi(lapic_nmi->int_number, lapic_nmi->flags);
                        }
                    }
                    
                    break;
                }
                
                for (uint64_t idx = 0; idx < core_num; ++idx)
                {
                    if (cores[idx].acpi_id() == lapic_nmi->acpi_id)
                    {
                        cores[idx].set_nmi(lapic_nmi->int_number, lapic_nmi->flags);
                        
                        break;
                    }
                    
                    if (idx == core_num - 1)
                    {
                        screen::print(tag::acpi, "Ignoring NMI vector entry for unknown ACPI ID ", lapic_nmi->acpi_id, "\n");
                    }
                }
                
                break;
            }
            
            
            case 10:
            {
                auto x2apic_nmi = (acpi::madt_x2apic_nmi_entry *)((uint64_t)entry + sizeof(*entry));
                
                if (x2apic_nmi->acpi_uuid == 0xffffffff)
                {
                    for (uint64_t i = 0; i < core_num; ++i)
                    {
                        cores[i].set_nmi(x2apic_nmi->int_number, x2apic_nmi->flags);
                    }
                    
                    break;
                }

                for (uint64_t idx = 0; idx < core_num; ++idx)
                {
                    if (cores[idx].acpi_id() == x2apic_nmi->acpi_uuid)
                    {
                        cores[idx].set_nmi(x2apic_nmi->int_number, x2apic_nmi->flags);
                        
                        break;
                    }
                    
                    if (idx == core_num - 1)
                    {
                        screen::print(tag::acpi, "Ignoring x2APIC NMI entry for unknown ACPI UUID ", x2apic_nmi->acpi_uuid, "\n");
                    }
                }
                
                break;
            }
        }
        
        entry = (acpi::madt_entry *)((uint64_t)entry + entry->length);
    }

    memory::vm::map(memory::vm::local_apic_address, lic_address);
}

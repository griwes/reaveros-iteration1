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

#include <acpi/acpi.h>
#include <memory/memory.h>
#include <memory/x64paging.h>
#include <acpi/tables.h>

namespace acpi
{
    rsdt * root = nullptr;
    xsdt * new_root = nullptr;
}

namespace
{
    void install_rsdt(acpi::rsdp * ptr)
    {
        memory::vas->map(0xFFFFC000, 0xFFFFFFFF, ptr->rsdt_ptr);
        
        if (((acpi::rsdt *)(0xFFFFC000 + ptr->rsdt_ptr % 4096))->validate("RSDT"))
        {
            acpi::root = (acpi::rsdt *)(0xFFFFC000 + ptr->rsdt_ptr % 4096);
            
            return;
        }
        
        else
        {
            PANIC("RSDT invalid");
        }
    }
    
    void install_xsdt(acpi::rsdp * ptr)
    {
        memory::vas->map(0xFFFFC000, 0xFFFFFFFF, ptr->xsdt_ptr);
        
        if (((acpi::xsdt *)(0xFFFFC000 + ptr->xsdt_ptr % 4096))->validate("XSDT"))
        {
            acpi::new_root = (acpi::xsdt *)(0xFFFFC000 + ptr->xsdt_ptr % 4096);
            
            return;
        }
        
        else
        {
            memory::vas->unmap(0xFFFFC000, 0xFFFFFFFF);
            
            screen::print(" (XSDT invalid, falling back to RSDT) ");
            install_rsdt(ptr);
        }
    }
    
    void install_root(acpi::rsdp * ptr)
    {
        if (ptr->revision)
        {
            install_xsdt(ptr);
        }
        
        else
        {
            install_rsdt(ptr);
        }
    }
}

template<>
void screen::print_impl(const acpi::rsdp & rsdp)
{
    screen::printl("Revision: ", rsdp.revision);
    screen::print("OEMID: ");
    screen::print<6>(rsdp.oemid);
    screen::line();
}

acpi::rsdp * acpi::find_rsdp()
{
    uint32_t ebda = *(uint16_t *)0x40E << 4;
    
    rsdp * ptr = (rsdp *)ebda;
    
    while (ptr < (rsdp *)(ebda + 1024))
    {        
        if (ptr->validate())
        {            
            install_root(ptr);
            
            return ptr;
        }
        
        else
        {
            ptr = (rsdp *)((uint32_t)ptr + 16);
        }
    }

    ptr = (rsdp *)0xe0000;
    
    while (ptr < (rsdp *)0x100000)
    {
        if (ptr->validate())
        {
            install_root(ptr);
            
            return ptr;
        }
        
        else
        {
            ptr = (rsdp *)((uint32_t)ptr + 16);
        }
    }
    
    PANIC("RSDP not found!");
    
    return nullptr;
}

processor::numa_env * acpi::find_numa_domains()
{
    srat * resources;
    
    if (new_root)
    {
        for (uint64_t i = 0; i < (new_root->length - 36) / 8; ++i)
        {
            resources = (srat *)(0xFFFF8000 + new_root->entries[i] % 4096);
            
            memory::vas->map(0xFFFF8000, 0xFFFFC000, new_root->entries[i]);
            
            if (resources->validate("SRAT"))
            {
                return new processor::numa_env(resources);
            }
            
            memory::vas->unmap(0xFFFF8000, 0xFFFFC000);
        }
    }
    
    else
    {
        for (uint64_t i = 0; i < (root->length - 36) / 4; ++i)
        {
            resources = (srat *)(0xFFFF8000 + root->entries[i] % 4096);
            
            memory::vas->map(0xFFFF8000, 0xFFFFC000, root->entries[i]);
            
            if (resources->validate("SRAT"))
            {
                return new processor::numa_env(resources);
            }
            
            memory::vas->unmap(0xFFFF8000, 0xFFFFC000);
        }
    }
    
    PANIC("SRAT not found");
    
    return nullptr;
}

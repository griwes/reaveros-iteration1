/**
 * Reaver Project OS, Rose License
 *
 * Copyright © 2013 Michał "Griwes" Dominiak
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
 **/

#pragma once

#include <memory/x64paging.h>
#include <memory/pmm.h>

namespace memory
{
    namespace vm
    {
        struct attributes
        {
            bool user = false;
            bool read_write = true;
            bool foreign = false;
        };

        constexpr static attributes user = { true, true };
        constexpr static attributes foreign_user = { true, true, true };
        constexpr static attributes user_read_only = { true, false };
        constexpr static attributes foreign_user_read_only = { true, false, true };
        constexpr static attributes system = {};
        constexpr static attributes foreign_system = { false, true, true };
        constexpr static attributes system_read_only = { false, false };
        constexpr static attributes foreign_system_read_only = { false, false, false };

        void initialize();
        virt_addr_t allocate_address_range(uint64_t size);

        inline void map(virt_addr_t address, attributes attrib = system)
        {
            x64::map(address, address + 4096, memory::pmm::pop(), attrib);
        }

        inline void map(virt_addr_t address, phys_addr_t physical, attributes attrib = system)
        {
            x64::map(address, address + 4096, physical, attrib);
        }

        inline void map(virt_addr_t base, virt_addr_t end, attributes attrib = system)
        {
            while (base < end)
            {
                x64::map(base, base + 4096, memory::pmm::pop(), attrib);

                base += 4096;
            }
        }

        inline void map(virt_addr_t base, virt_addr_t end, phys_addr_t physical, attributes attrib = system)
        {
            x64::map(base, end, physical, attrib);
        }

        inline void unmap(virt_addr_t base, virt_addr_t end, bool push_frames = true, bool foreign = false)
        {
            x64::unmap(base, end, push_frames, foreign);
        }

        inline phys_addr_t get_physical_address(virt_addr_t virtual_address)
        {
            return x64::get_physical_address(virtual_address);
        }

        inline phys_addr_t clone_kernel()
        {
            return x64::clone_kernel();
        }

        inline void set_foreign(phys_addr_t foreign)
        {
            x64::set_foreign(foreign);
        }

        inline void release_foreign()
        {
            x64::release_foreign();
        }
    }
}
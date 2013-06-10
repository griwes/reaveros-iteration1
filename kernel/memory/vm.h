/**
 * Reaver Project OS, Rose License
 *
 * Copyright (C) 2013 Reaver Project Team:
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

#include <memory/x64paging.h>

namespace memory
{
    namespace vm
    {
        void initialize();
        uint64_t allocate_address_range(uint64_t size);

        inline void map(uint64_t address, uint64_t physical)
        {
            x64::map(address, address + 4096, physical);
        }

        inline void map_multiple(uint64_t base, uint64_t end, uint64_t physical)
        {
            x64::map(base, end, physical);
        }

        inline uint64_t get_physical_address(uint64_t virtual_address)
        {
            return x64::get_physical_address(virtual_address);
        }
    }
}
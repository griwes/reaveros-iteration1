/**
 * Reaver Project OS, Rose License
 *
 * Copyright © 2011-2012 Michał "Griwes" Dominiak
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

namespace memory
{
    namespace x64
    {
        struct page_table_entry
        {
            page_table_entry & operator=(uint64_t addr)
            {
                present = 1;
                read_write = 1;
                address = addr >> 12;

                return *this;
            }

            uint64_t present:1;
            uint64_t read_write:1;
            uint64_t user:1;
            uint64_t write_through:1;
            uint64_t cache_disable:1;
            uint64_t accessed:1;
            uint64_t dirty:1;
            uint64_t pat:1;
            uint64_t global:1;
            uint64_t ignored:3;
            uint64_t address:40;
            uint64_t ignored2:11;
            uint64_t reserved:1;
        };

        struct page_table
        {
            page_table()
            {
                for (uint32_t i = 0; i < 512; ++i)
                {
                    entries[i] = page_table_entry{};
                }
            }

            page_table_entry & operator[](uint64_t i)
            {
                if (i < 512)
                {
                    return entries[i];
                }

                else
                {
                    PANIC("PT entry access out of bounds");

                    return *(page_table_entry *)0;
                }
            }

            page_table_entry entries[512];
        } __attribute__((__packed__));

        struct page_directory_entry
        {
            page_directory_entry & operator=(page_table * pt)
            {
                present = 1;
                read_write = 1;
                address = (uint64_t)pt >> 12;

                return *this;
            }

            uint64_t present:1;
            uint64_t read_write:1;
            uint64_t user:1;
            uint64_t write_through:1;
            uint64_t cache_disable:1;
            uint64_t accessed:1;
            uint64_t ignored:1;
            uint64_t reserved:1;
            uint64_t ignored2:4;
            uint64_t address:40;
            uint64_t ignored3:11;
            uint64_t reserved2:1;
        };

        struct page_directory
        {
            page_directory()
            {
                for (uint32_t i = 0; i < 512; ++i)
                {
                    entries[i] = page_directory_entry{};
                }
            }

            page_directory_entry & operator[](uint64_t i)
            {
                if (i < 512)
                {
                    return entries[i];
                }

                else
                {
                    PANIC("PD entry access out of bounds");

                    return *(page_directory_entry *)0;
                }
            }

            page_directory_entry entries[512];
        } __attribute__((__packed__));

        struct pdpt_entry
        {
            pdpt_entry & operator=(page_directory * pd)
            {
                present = 1;
                read_write = 1;
                address = (uint64_t)pd >> 12;

                return *this;
            }

            uint64_t present:1;
            uint64_t read_write:1;
            uint64_t user:1;
            uint64_t write_through:1;
            uint64_t cache_disable:1;
            uint64_t accessed:1;
            uint64_t ignored:1;
            uint64_t reserved:1;
            uint64_t ignored2:4;
            uint64_t address:40;
            uint64_t ignored3:11;
            uint64_t reserved2:1;
        };

        struct pdpt
        {
            pdpt()
            {
                for (uint32_t i = 0; i < 512; ++i)
                {
                    entries[i] = pdpt_entry{};
                }
            }

            pdpt_entry & operator[](uint64_t i)
            {
                if (i < 512)
                {
                    return entries[i];
                }

                else
                {
                    PANIC("PDPT entry access out of bounds");

                    return *(pdpt_entry *)0;
                }
            }

            pdpt_entry entries[512];
        } __attribute__((__packed__));

        struct pml4;

        struct pml4_entry
        {
            pml4_entry & operator=(pdpt * table)
            {
                present = 1;
                read_write = 1;
                address = (uint64_t)table >> 12;

                return *this;
            }

            uint64_t present:1;
            uint64_t read_write:1;
            uint64_t user:1;
            uint64_t write_through:1;
            uint64_t cache_disable:1;
            uint64_t accessed:1;
            uint64_t ignored:1;
            uint64_t reserved:1;
            uint64_t ignored2:4;
            uint64_t address:40;
            uint64_t ignored3:11;
            uint64_t reserved2:1;
        };

        struct pml4
        {
            pml4()
            {
                for (uint32_t i = 0; i < 512; ++i)
                {
                    entries[i] = pml4_entry{};
                }
            }

            void map(uint64_t, uint64_t, uint64_t);
            void unmap(uint64_t, uint64_t);

            pml4_entry & operator[](uint64_t i)
            {
                if (i < 512)
                {
                    return entries[i];
                }

                else
                {
                    PANIC("PML4 entry access out of bounds");

                    return *(pml4_entry *)0;
                }
            }

            pml4_entry entries[512];
        };
    }
}

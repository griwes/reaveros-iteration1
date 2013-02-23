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

#include <memory/x64paging.h>
#include <memory/pmm.h>
#include <memory/aligner.h>
#include <scheduler/process.h>
#include <scheduler/thread.h>

namespace memory
{
    namespace vm
    {
        enum addresses
        {
            acpi_temporal_rsdt_mapping = 0xFFFFFFFFFFFF0000 - 0x8000,
            acpi_temporal_table_mapping = acpi_temporal_rsdt_mapping - 0x8000,
            local_apic_address = acpi_temporal_table_mapping - 0x1000,
            global_frame_stack = 0xFFFFFFFF40000000 - memory::max_memory_supported / (4096) * 8,
            boot_backbuffer = global_frame_stack - 0x40000000,
            boot_video_memory = boot_backbuffer - 0x40000000,
            ioapic_area = boot_video_memory - processor::max_ioapics * 4096,
            frame_stack_area = ioapic_area - memory::stack_size * processor::max_cores,
            global_stack_stack_area = frame_stack_area - scheduler::max_threads * 8,
            stack_stack_area = global_stack_stack_area - memory::stack_size * processor::max_cores,
            stack_area = stack_stack_area - memory::stack_size * 2 * 4096,
            global_pcb_stack_area = stack_area - scheduler::max_processes * 8,
            global_tcb_stack_area = global_pcb_stack_area - scheduler::max_processes * 8,
            pcb_stack_area = global_tcb_stack_area - memory::core_index_stack_size,
            tcb_stack_area = pcb_stack_area - memory::core_index_stack_size,
            pcb_area = tcb_stack_area - scheduler::max_processes * aligner<scheduler::process>::size,
            tcb_area = pcb_area - scheduler::max_threads * aligner<scheduler::thread>::size
        };

        inline void map(uint64_t virtual_address)
        {
            x64::map(virtual_address, virtual_address + 4096, memory::pmm::pop());
        }

        inline void map(uint64_t virtual_address, uint64_t physical_address)
        {
            x64::map(virtual_address, virtual_address + 4096, physical_address);
        }

        void map_multiple(uint64_t, uint64_t);

        inline void map_multiple(uint64_t virtual_start, uint64_t virtual_end, uint64_t physical_start)
        {
            x64::map(virtual_start, virtual_end, physical_start);
        }

        inline uint64_t get_physical_address(uint64_t virtual_address)
        {
            return x64::get_physical_address(virtual_address);
        }

        inline void unmap(uint64_t address, bool push = true)
        {
            x64::unmap(address, (address + 4095) & ~(uint64_t)4096, push);
        }

        inline void unmap(uint64_t start, uint64_t end, bool push = true)
        {
            x64::unmap(start, end, push);
        }

        inline bool locked(uint64_t address)
        {
            return x64::locked(address);
        }
    }
}

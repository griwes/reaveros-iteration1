/**
 * Reaver Project OS, Rose License
 *
 * Copyright © 2011-2014 Michał "Griwes" Dominiak
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

#include <memory/vm.h>
#include <processor/gdt.h>
#include <memory/stack.h>
#include <time/lapic.h>
#include <scheduler/local.h>

namespace processor
{
    extern "C" void ap_initialize();
    void set_current_thread(scheduler::thread *);
    void handle(processor::isr_context &);

    namespace gdt
    {
        void ap_initialize();
    }

    class core;

    namespace smp
    {
        void boot(core *, uint64_t);
    }

    class core
    {
    public:
        core() : _is_valid{}, _is_nmi_valid{} {}

        core(uint32_t apic_id, uint32_t acpi_id, bool is_lapic = true) : _acpi_id{ acpi_id }, _apic_id{ apic_id },
            _is_local_apic{ is_lapic }, _is_valid{ true }, _is_nmi_valid{}
        {
        }

        core & operator=(const core & rhs)
        {
            _acpi_id = rhs._acpi_id;
            _apic_id = rhs._apic_id;
            _is_local_apic = rhs._is_local_apic;
            _nmi_vector = rhs._nmi_vector;
            _nmi_flags = rhs._nmi_flags;
            _is_valid = rhs._is_valid;
            _is_nmi_valid = rhs._is_nmi_valid;

            return *this;
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

        memory::pmm::frame_stack & frame_stack()
        {
            return _frame_stack;
        }

        time::lapic::timer & preemption_timer()
        {
            return _timer;
        }

        ::scheduler::local & scheduler()
        {
            return _scheduler;
        }

        bool in_interrupt_handler()
        {
            return _is_in_interrupt;
        }

        friend void processor::ap_initialize();
        friend void processor::gdt::ap_initialize();
        friend void processor::smp::boot(core *, uint64_t);
        friend void processor::set_current_thread(scheduler::thread *);
        friend void processor::handle(processor::isr_context &);

        // DO NOT MOVE THIS AROUND
        // this member must be the first member of core, memory wise
        // it's used in kernel-side thread-local storage
        // (GS_BASE points to current core's `core`, so [gs:0] is used to get the pointer to the current thread,
        // hence the offset of this member must be 0)
        ::scheduler::thread * thread = nullptr;

    private:
        uint32_t _acpi_id;
        uint32_t _apic_id;
        bool _is_local_apic;

        uint32_t _nmi_vector;
        uint32_t _nmi_flags;

        bool _is_valid;
        bool _is_nmi_valid;

        bool _is_in_interrupt = false;

        processor::gdt::gdt_entry _gdt[7];
        processor::gdt::tss _tss;
        processor::gdt::gdtr _gdtr;

        volatile uint8_t * _started;

        memory::pmm::frame_stack _frame_stack;
        time::lapic::timer _timer{ nullptr };

        ::scheduler::local _scheduler{ nullptr };
    };
}
/**
* Reaver Project OS, Rose License
*
* Copyright © 2014 Michał "Griwes" Dominiak
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

#include <processor/syscalls.h>
#include <screen/screen.h>
#include <processor/context.h>
#include <scheduler/thread.h>
#include <scheduler/process.h>
#include <utils/hash_map.h>

namespace
{
    constexpr uint64_t _star = 0xC0000081;
    constexpr uint64_t _lstar = 0xC0000082;
    constexpr uint64_t _cstar = 0xC0000083;
    constexpr uint64_t _sfmask = 0xC0000084;
    constexpr uint64_t _ia32_efer = 0xC0000080;

    bool _is_canonical(uint64_t address)
    {
        bool bit = (address >> 47) & 1;

        if (bit && (address >> 48) == 0xffff)
        {
            return true;
        }

        if (!bit && (address >> 48) == 0)
        {
            return true;
        }

        return false;
    }

    struct _handler_description
    {
        _handler_description(const _handler_description &) = default;
        _handler_description(_handler_description &&) = default;
        _handler_description & operator=(const _handler_description &) = default;
        _handler_description & operator=(_handler_description &&) = default;

        _handler_description(processor::syscalls::handler hnd = nullptr, uint64_t ctx = 0, bool srv_only = false) : handler{ hnd }, context{ ctx }, service_only{ srv_only }
        {
        }

        processor::syscalls::handler handler;
        uint64_t context = 0;
        bool service_only = false;
    };

    struct _syscall_hash
    {
        uint64_t operator()(processor::syscalls::syscalls syscall_number) const
        {
            return static_cast<uint64_t>(syscall_number);
        }
    };

    utils::lazy<utils::hash_map<processor::syscalls::syscalls, _handler_description, _syscall_hash, 23>> _handlers;

    // so here goes the x86_64 syscall abi documentation
    // TODO: this should possibly be placed in some better place

    // Register interpretation at syscall:
    //  - syscall number: rax
    //  - return address: rcx (automatically set, the old value of rcx is discarded)
    //  - userspace stack: rdx (semi-automatically set, the old value or rdx is discarded)
    //  - arguments: rsi, rdi, r8, r9, r10, r12 up to r15

    // Register status at return:
    //  - return value: rax
    //  - rcx: return address
    //  - rest of registers: saved values
    extern "C" void _syscall_handler(processor::syscall_context context)
    {
        if (unlikely(!scheduler::ready()))
        {
            PANIC("syscall handler called before scheduler got initialized.");
        }

        scheduler::thread * calling_thread = scheduler::current_thread();
        uint64_t tid = calling_thread->id;

        auto syscall_number = static_cast<processor::syscalls::syscalls>(context.syscall_number);

        if (!_handlers->contains(syscall_number))
        {
            TODOEX("a process tried to call a non-existent syscall");
        }

        const auto & handler = (*_handlers)[syscall_number];

        if (handler.service_only && !calling_thread->parent->service)
        {
            TODOEX("a non-service attempted a call to a service-only syscall");
        }

        handler.handler(handler.context, context);

        if (tid && scheduler::current_thread()->id != tid)
        {
            if (scheduler::valid(calling_thread))
            {
                calling_thread->save(context);
            }

            scheduler::current_thread()->load(context);
        }

        if (_is_canonical(context.user_rip))
        {
            return;
        }

        TODOEX("a process has just general protection faulted trying to sysretq to non canonical address; implement this");
    }
}

extern "C" void syscall_handler_entry();

void processor::syscalls::initialize()
{
    wrmsr(_ia32_efer, rdmsr(_ia32_efer) | 1);
    wrmsr(_lstar, reinterpret_cast<uint64_t>(&syscall_handler_entry));
    wrmsr(_star, (8ull << 32) | (8ull << 48));

    _handlers.initialize();

    register_syscall(syscalls::service_kernel_console_print, [](uint64_t, syscall_context & context)
    {
        screen::print(reinterpret_cast<const char *>(context.rsi));
    }, 0, true);
}

void processor::syscalls::ap_initialize()
{
    wrmsr(_ia32_efer, rdmsr(_ia32_efer) | 1);
    wrmsr(_lstar, reinterpret_cast<uint64_t>(&syscall_handler_entry));
    wrmsr(_star, (8ull << 32) | (8ull << 48));
}

void processor::syscalls::register_syscall(processor::syscalls::syscalls syscall, processor::syscalls::handler hnd, uint64_t context, bool service_only)
{
    _handlers->insert(syscall, { hnd, context, service_only });
}

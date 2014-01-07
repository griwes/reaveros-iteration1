/**
 * Reaver Project OS, Rose License
 *
 * Copyright (C) 2013-2014 Reaver Project Team:
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

#include <memory/pmm.h>
#include <memory/map.h>
#include <memory/stack.h>
#include <memory/vm.h>
#include <screen/screen.h>
#include <processor/core.h>
#include <processor/smp.h>
#include <processor/processor.h>

memory::pmm::frame_stack _global_stack;

namespace
{
    uint8_t _boot_frames[8 * 4096] __attribute__((aligned(4096)));
    uint8_t _boot_frames_available = 8;
    uint64_t _boot_frames_start = 0;

    memory::map_entry * _map;
    uint64_t _map_size;

    std::atomic<uint64_t> _amount{ 0 };
}

void memory::pmm::initialize(memory::map_entry * map, uint64_t map_size)
{
    _boot_frames_start = vm::get_physical_address((uint64_t)_boot_frames);

    new (&_global_stack) frame_stack{ map, map_size };

    _map = map;
    _map_size = map_size;
}

void memory::pmm::ap_initialize()
{
    screen::debug("\nInitializing local frame stack for core #", processor::id());

    if (!_amount)
    {
        _amount = _global_stack.size() / (2 * processor::get_core_count() * frame_stack_chunk::max);
    }

    uint64_t amount = _amount;
    auto & core = *processor::get_current_core();
    core.frame_stack().set_balancing(amount * 3 / 2);

    while (amount--)
    {
        core.frame_stack().push_chunk(_global_stack.pop_chunk());
    }
}

uint64_t memory::pmm::pop()
{
    if (_boot_frames_available)
    {
        screen::debug("\nPopping one of ", _boot_frames_available, " available boot frames.");
        return _boot_frames_start + (8 - _boot_frames_available--) * 4096;
    }

    if (unlikely(!processor::smp::ready()))
    {
        return _global_stack.pop();
    }

    return processor::get_current_core()->frame_stack().pop();
}

void memory::pmm::push(uint64_t frame)
{
    if (unlikely(!processor::smp::ready()))
    {
        _global_stack.push(frame);
        screen::debug("\nPushed ", (void *)frame, " to global frame stack on #", processor::id());
        return;
    }

    processor::get_current_core()->frame_stack().push(frame);
    screen::debug("\nPushed ", (void *)frame, " to local frame stack on #", processor::id());
}

void memory::pmm::boot_report()
{
    screen::print("Free memory: ", (_global_stack.size() * 4096) / (1024 * 1024 * 1024), " GiB ", ((_global_stack.size() * 4096) %
        (1024 * 1024 * 1024)) / (1024 * 1024), " MiB ", ((_global_stack.size() * 4096) % (1024 * 1024)) / 1024, " KiB", '\n');
    screen::print("Total usable memory detected at boot: ");

    uint64_t total = 0;

    for (uint64_t i = 0; i < _map_size; ++i)
    {
        if (_map[i].type != 6 && _map[i].type != 8 && _map[i].type != 9)
        {
            total += _map[i].length;
        }
    }

    screen::print(total / (1024 * 1024 * 1024), " GiB ", (total % (1024 * 1024 * 1024)) / (1024 * 1024), " MiB ",
        (total % (1024 * 1024)) / 1024, " KiB", '\n', '\n');
}
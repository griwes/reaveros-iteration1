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

#include <memory/pmm.h>
#include <memory/map.h>
#include <memory/stack.h>
#include <memory/vm.h>

namespace
{
    memory::pmm::frame_stack _global_stack;

    uint8_t _boot_frames[3 * 4096] __attribute__((aligned(4096)));
    uint8_t _boot_frames_available = 3;
    uint64_t boot_frames_start = 0;
}

void memory::pmm::initialize(memory::map_entry * map, uint64_t map_size)
{
    boot_frames_start = vm::get_physical_address((uint64_t)_boot_frames);

    new (&_global_stack) frame_stack{ map, map_size };
}

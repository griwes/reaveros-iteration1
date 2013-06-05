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

#include <memory/memory.h>
#include <memory/x64paging.h>
#include <processor/processor.h>

namespace
{
    screen::mode _mode;
    memory::map_entry _map[512] = {};
    uint8_t _font[4096];
}

void memory::copy_bootloader_data(screen::mode *& video, memory::map_entry *& entries, uint64_t size)
{
    _mode = *video;
    memory::copy(entries, _map, size);
    memory::copy(_mode.font, _font, 4096);
    _mode.font = _font;

    video = &_mode;
    entries = _map;

    return;
}

void memory::initialize_paging()
{
    x64::pml4 * boot_vas = processor::get_cr3();

    (*boot_vas)[256] = (uint64_t)boot_vas;

    processor::reload_cr3();
}

void memory::drop_bootloader_mapping(bool push)
{
    memory::x64::drop_bootloader_mapping(push);
}

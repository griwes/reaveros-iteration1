/**
 * Reaver Project OS, Rose License
 *
 * Copyright © 2011-2013 Michał "Griwes" Dominiak
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

#include <screen/screen.h>
#include <screen/mode.h>
#include <screen/console.h>
#include <screen/bootterm.h>

void screen::initialize_console()
{
    new ((void *)&console) kernel_console{ nullptr };
}

void screen::initialize_terminal(screen::mode * video_mode, memory::map_entry * map, uint64_t map_size)
{
    new ((void *)&term) boot_terminal{ video_mode, map, map_size };

    console.set_terminal(&term);
}

void screen::print(tag::tags t)
{
    auto _ = console.lock();

    screen::print(color::white);

    switch (t)
    {
        case tag::memory:
            screen::print("[Memory   ] ");
            break;
        case tag::cpu:
            screen::print("[CPU      ] ");
            break;
        case tag::acpi:
            screen::print("[ACPI     ] ");
            break;
        case tag::scheduler:
            screen::print("[Scheduler] ");
    }

    screen::print(color::gray);
}

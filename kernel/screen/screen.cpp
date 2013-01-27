/**
 * Reaver Project OS, Rose License
 * 
 * Copyright (C) 2011-2012 Reaver Project Team:
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

#include <screen/screen.h>
#include <screen/mode.h>
#include <screen/console.h>
#include <screen/bootterm.h>

void screen::initialize(screen::mode * video_mode, memory::map_entry * map, uint64_t map_size)
{
    new ((void *)&term) boot_terminal(video_mode, map, map_size);
    new ((void *)&console) kernel_console(&term);
}

void screen::print(tag::tags t)
{
    screen::print(color::white);
    
    switch (t)
    {
        case tag::memory:
            screen::print("[Memory] ");
            break;
        case tag::cpu:
            screen::print("[CPU   ] ");
            break;
        case tag::acpi:
            screen::print("[ACPI  ] ");
    }
    
    screen::print(color::gray);
}

void screen::transaction()
{
    screen::console.transaction();
}

void screen::commit()
{
    screen::console.commit();
}

void screen::clear()
{
    screen::console.clear();
}

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

#include <screen/screen.h>
#include <screen/mode.h>
#include <screen/console.h>
#include <screen/terminal.h>
#include <utils/lazy.h>

namespace memory
{
    struct map_entry;
}

namespace screen
{
    class boot_terminal : public terminal
    {
    public:
        boot_terminal() {}
        boot_terminal(mode *, memory::map_entry *, uint64_t);
        virtual ~boot_terminal();

        virtual void clear();

        virtual void put_char(char);

        virtual void set_color(color::colors);

    private:
        void _put_16(char);
        void _put_32(char);
        void _scroll();

        screen::mode * _mode;
        virt_addr_t _backbuffer;

        uint32_t _maxx, _maxy;
        uint32_t _x, _y;

        uint8_t _red, _green, _blue;

        utils::spinlock _lock;
    };

    extern utils::lazy<boot_terminal> term;
}

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

#pragma once

#include <screen/mode.h>

namespace memory
{
    class map;
}

namespace screen
{
    // a bit different than kernel's console-terminal-driver scheme, but this class
    // encapsulates font and mode data, to keep it away from everyone including 
    // screen headers, unlike booter 0.2, where that data was freely available to
    // any file
    
    // unlike kernel's console-terminal-driver scheme, here the console is only
    // meant to write individual characters to screen, conversions from ints or 
    // structs to chars is done with screen::print[X[X]]() calls
    
    // proper console-terminal-driver scheme, with full standard stream interface,
    // will be available in kernel only - I don't want to implement the same full
    // scheme in both booter and kernel, and I don't want to link kernel-mode stdlib
    // to booter as well, as booter doesn't provide proper environment for stdlib
    
    // that said, time to start writing librose stdlib implementation...
    
    class console
    {
    public:
        console(boot_mode *, void *);
        ~console();
        
        void init_backbuffer(memory::map &);
        
        void put_char(char);
        void save_backbuffer_info(memory::map *);
        
        void print_mode_info();
        
        uint32_t video_start()
        {
            return _mode.addr;
        }
        
        uint32_t video_end()
        {
            return _mode.addr + _y * _mode.bytes_per_line;
        }
        
    private:
        void _put_16(char);
        void _put_32(char);
        
        void _scroll();
        void _clear();
        
        mode _mode;
        uint8_t * _font;
        
        uint16_t _x, _y;
        uint16_t _maxx, _maxy;
        
        uint32_t _backbuffer;
    };
}

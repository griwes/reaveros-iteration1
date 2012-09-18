#pragma once

#include "mode.h"

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
        
        void put_char(char);
        void save_backbuffer_info(memory::map *);
        
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

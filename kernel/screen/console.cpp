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

#include <screen/console.h>
#include <screen/terminal.h>

namespace screen
{
    kernel_console console;
}

screen::kernel_console::kernel_console(terminal * term) : _terminal(term)
{
}

void screen::kernel_console::clear()
{
    _terminal->clear();
}

void screen::kernel_console::print(char c)
{
    _terminal->put_char(c);
    
    if (c != '\0')
    {
        outb(0x378, (unsigned char)c);
        outb(0x37a, 0x0c);
        outb(0x37a, 0x0d);
    }
}

void screen::kernel_console::print(const char * str)
{
    while (*str)
    {
        outb(0x378, (unsigned char)*str);
        outb(0x37a, 0x0c);
        outb(0x37a, 0x0d);
        
        _terminal->put_char(*str++);
    }
}

void screen::kernel_console::set_color(color::colors c)
{
    _terminal->set_color(c);
}

namespace
{
    template<typename T>
    void _print_int(T t)
    {
        if (t == 0)
        {
            screen::console.print('0');
            
            return;
        }
        
        if (!(t > 0 || t == 0)) // supress [T = unsigned] unsigned comparison t < 0 warning
        {
            screen::console.print('-');
            t = -t;
        }
        
        T mod = t % 10;
        
        if (t >= 10)
        {
            _print_int(t / 10);
        }
        
        screen::console.print('0' + mod);
    }
}

void screen::kernel_console::print(int8_t i)
{
    _print_int(i);
}

void screen::kernel_console::print(int16_t i)
{
    _print_int(i);
}

void screen::kernel_console::print(int32_t i)
{
    _print_int(i);
}

void screen::kernel_console::print(int64_t i)
{
    _print_int(i);
}

void screen::kernel_console::print(uint8_t i)
{
    _print_int(i);
}

void screen::kernel_console::print(uint16_t i)
{
    _print_int(i);
}

void screen::kernel_console::print(uint32_t i)
{
    _print_int(i);
}

void screen::kernel_console::print(uint64_t i)
{
    _print_int(i);
}

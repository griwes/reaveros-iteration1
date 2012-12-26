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

#include <screen/console.h>
#include <memory/memory.h>
#include <memory/manager.h>

screen::console::console(screen::boot_mode * mode, void * font)
    : _mode(mode), _font((uint8_t *)font), _x(0), _y(0), _maxx(mode->resolution_x / 8), _maxy(mode->resolution_y / 16),
      _backbuffer(0)
{
    _clear();
}

screen::console::~console()
{
}

void screen::console::init_backbuffer()
{
    _backbuffer = (uint32_t)((memory::manager::backwards_allocator()).allocate(_mode.bytes_per_line * _mode.resolution_y));
    memory::zero((uint8_t *)_backbuffer, _mode.resolution_y * _mode.bytes_per_line);
}

void screen::console::save_backbuffer_info()
{
    memory::map_entry backbuffer{};
    backbuffer.base = _backbuffer;
    backbuffer.length = _mode.resolution_y * _mode.bytes_per_line;
    backbuffer.length += 4096 - backbuffer.length % 4096;
    backbuffer.type = 4;
    memory::map::add_entry(backbuffer);
}

void screen::console::put_char(char c)
{
    if (c != '\0')
    {
        outb(0x378, (unsigned char)c);
        outb(0x37a, 0x0c);
        outb(0x37a, 0x0d);
    }
    
    switch (c)
    {
        case '\0':
            return;
            
        case '\n':
            _x = 0;
            _y++;
            
            if (_y >= _maxy)
            {
                _scroll();
            }
            
            return;
            
        case '\r':
            _x = 0;
            
            return;
            
        case '\t':
            _x += 8 - _x % 8;
            
            return;
    }
    
    
    switch (_mode.bpp)
    {
        case 16:
            _put_16(c);
            
            break;
            
        case 32:
            _put_32(c);
            
            break;
    }    
    _x++;
    
    if (_x == _maxx)
    {
        _x = 0;
        _y++;
        
        if (_y == _maxy)
        {
            _scroll();
        }
    }    
}

void screen::console::_put_16(char c)
{
    uint8_t * character = &(_font[c * 16]);
    uint16_t * dest = (uint16_t *)(_mode.addr + _y * _mode.bytes_per_line * 16 + _x * 16);
    
    uint16_t * backdest;
    if (_backbuffer)
    {
        backdest = (uint16_t *)(_backbuffer + _y * _mode.bytes_per_line * 16 + _x * 16);
    }
    else
    {
        backdest = dest;
    }
    
    uint16_t color = ((0xc0 >> (8 - _mode.red_size)) << _mode.red_pos) | ((0xc0 >> (8 - _mode.green_size)) 
        << _mode.green_pos) | ((0xc0 >> (8 - _mode.blue_size)) << _mode.blue_pos);
    
    for (uint64_t i = 0; i < 16; i++)
    {
        uint8_t data = character[i];
        
        for (uint64_t i = 0; i < 8; i++)
        {
            backdest[i] = (data >> (7 - i)) & 1 ? color : 0;
            dest[i] = backdest[i];
        }
                
        uint32_t _ = (uint32_t)dest;
        _ += _mode.bytes_per_line;
        dest = (uint16_t *)_;
        
        _ = (uint32_t)backdest;
        _ += _mode.bytes_per_line;
        backdest = (uint16_t *)_;
    }
}

void screen::console::_put_32(char c)
{
    uint8_t * character = &(_font[c * 16]);
    uint32_t * dest = (uint32_t *)(_mode.addr + _y * _mode.bytes_per_line * 16 + _x * 32);
    
    uint32_t * backdest;
    if (_backbuffer)
    {
        backdest = (uint32_t *)(_backbuffer + _y * _mode.bytes_per_line * 16 + _x * 32);
    }
    else
    {
        backdest = dest;
    }
    
    uint32_t color = ((0xc0 >> (8 - _mode.red_size)) << _mode.red_pos) | ((0xc0 >> (8 - _mode.green_size)) 
        << _mode.green_pos) | ((0xc0 >> (8 - _mode.blue_size)) << _mode.blue_pos);
    
    for (uint64_t i = 0; i < 16; i++)
    {
        uint8_t data = character[i];
        
        for (uint64_t i = 0; i < 8; i++)
        {
            backdest[i] = (data >> (7 - i)) & 1 ? color : 0;
            dest[i] = backdest[i];
        }
        
        uint32_t _ = (uint32_t)dest;
        _ += _mode.bytes_per_line;
        dest = (uint32_t *)_;
        
        _ = (uint32_t)backdest;
        _ += _mode.bytes_per_line;
        backdest = (uint32_t *)_;
    }
}

void screen::console::_clear()
{
    memory::zero((uint8_t *)_mode.addr, _mode.resolution_y * _mode.bytes_per_line);
    if (_backbuffer)
    {
        memory::zero((uint8_t *)_backbuffer, _mode.resolution_y * _mode.bytes_per_line);
    }
}

void screen::console::_scroll()
{
    memory::copy((uint8_t *)_backbuffer + _mode.bytes_per_line * 16, (uint8_t *)_backbuffer, (_mode.resolution_y - 16
        - _mode.resolution_y % 16) * _mode.bytes_per_line);
    memory::zero((uint8_t *)_backbuffer + _mode.bytes_per_line * (_mode.resolution_y - 16 - _mode.resolution_y % 16), 
        _mode.bytes_per_line * 16);
    memory::copy((uint8_t *)_backbuffer, (uint8_t *)_mode.addr, _mode.bytes_per_line * _mode.resolution_y);
    
    --_y;
}

void screen::console::print_mode_info()
{
    screen::printl("Resolution: ", _mode.resolution_x, "x", _mode.resolution_y);
    screen::printl("Bits per pixel: ", _mode.bpp);
    screen::printf("Video memory: 0x%016x", video_start()); screen::printfl(" - 0x%016x", video_end());
    screen::line();
}

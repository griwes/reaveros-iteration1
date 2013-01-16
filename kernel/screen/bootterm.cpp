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

#include <screen/bootterm.h>
#include <memory/memory.h>
#include <memory/vm.h>

namespace screen
{
    boot_terminal term;
}

uint64_t _find_backbuffer(memory::map_entry * map, uint64_t map_size)
{
    for (uint64_t i = 0; i < map_size; ++i)
    {
        if (map[i].type == 4)
        {
            memory::vm::map_multiple(0xFFFFFFFF40000000, 0xFFFFFFFF40000000 + map[i].length, map[i].base);

            return 0xFFFFFFFF40000000;
        }
    }
    
    return 0;
}

screen::boot_terminal::boot_terminal(screen::mode * mode, memory::map_entry * map, uint64_t map_size)
    : _mode(mode), _backbuffer(_find_backbuffer(map, map_size)), _maxx(mode->resolution_x / 8), _maxy(mode->resolution_y / 16),
      _x(0), _y(0)
{
    memory::vm::map_multiple(0xFFFFFFFF00000000, 0xFFFFFFFF00000000 + mode->resolution_y * mode->bytes_per_line, mode->short_addr);
    _mode->addr = 0xFFFFFFFF00000000;
    
    clear();
    
    set_color(color::gray);
}

screen::boot_terminal::~boot_terminal()
{
}

void screen::boot_terminal::clear()
{
    if (_backbuffer)
    {
        memory::zero((uint8_t *)_backbuffer, _mode->resolution_y * _mode->bytes_per_line);
    }
    
    memory::zero((uint8_t *)(uint64_t)_mode->addr, _mode->resolution_y * _mode->bytes_per_line);
    
    _x = 0;
    _y = 0;
}

void screen::boot_terminal::put_char(char c)
{
    if (c == '\n')
    {
        ++_y;
        _x = 0;
    }
    
    else if (c == '\t')
    {
        _x += 8 - _x % 8;
    }
    
    else 
    {
        if (_mode->bpp == 16)
        {
            _put_16(c);
        }
        
        else if (_mode->bpp == 32)
        {
            _put_32(c);
        }
        
        ++_x;
    }
    
    if (_x == _maxx)
    {
        _x = 0;
        ++_y;
    }
    
    while (_y >= _maxy)
    {
        _scroll();
    }
}

void screen::boot_terminal::_put_16(char c)
{
    uint8_t * character = &(_mode->font[c * 16]);
    uint16_t * dest = (uint16_t *)((uint64_t)_mode->addr + _y * _mode->bytes_per_line * 16 + _x * 16);
    
    uint16_t * backdest;
    if (_backbuffer)
    {
        backdest = (uint16_t *)(_backbuffer + _y * _mode->bytes_per_line * 16 + _x * 16);
    }
    else
    {
        backdest = dest;
    }
    
    uint16_t color = ((_red >> (8 - _mode->red_size)) << _mode->red_pos) | ((_green >> (8 - _mode->green_size)) 
        << _mode->green_pos) | ((_blue >> (8 - _mode->blue_size)) << _mode->blue_pos);
    
    for (uint64_t i = 0; i < 16; i++)
    {
        uint8_t data = character[i];
        
        for (uint64_t i = 0; i < 8; i++)
        {
            backdest[i] = (data >> (7 - i)) & 1 ? color : 0;
            dest[i] = backdest[i];
        }
        
        uint64_t _ = (uint64_t)dest;
        _ += _mode->bytes_per_line;
        dest = (uint16_t *)_;
        
        _ = (uint64_t)backdest;
        _ += _mode->bytes_per_line;
        backdest = (uint16_t *)_;
    }
}

void screen::boot_terminal::_put_32(char c)
{    
    uint8_t * character = &(_mode->font[c * 16]);
    uint32_t * dest = (uint32_t *)((uint64_t)_mode->addr + _y * _mode->bytes_per_line * 16 + _x * 32);
    
    uint32_t * backdest;
    if (_backbuffer)
    {
        backdest = (uint32_t *)(_backbuffer + _y * _mode->bytes_per_line * 16 + _x * 32);
    }
    else
    {
        backdest = dest;
    }
    
    uint32_t color = ((_red >> (8 - _mode->red_size)) << _mode->red_pos) | ((_green >> (8 - _mode->green_size)) 
        << _mode->green_pos) | ((_blue >> (8 - _mode->blue_size)) << _mode->blue_pos);
    
    for (uint64_t i = 0; i < 16; i++)
    {
        uint8_t data = character[i];
        
        for (uint64_t i = 0; i < 8; i++)
        {
            backdest[i] = (data >> (7 - i)) & 1 ? color : 0;
            dest[i] = backdest[i];
        }
        
        uint64_t _ = (uint64_t)dest;
        _ += _mode->bytes_per_line;
        dest = (uint32_t *)_;
        
        _ = (uint64_t)backdest;
        _ += _mode->bytes_per_line;
        backdest = (uint32_t *)_;
    }
}

void screen::boot_terminal::_scroll()
{
    memory::copy((uint8_t *)_backbuffer + _mode->bytes_per_line * 16, (uint8_t *)_backbuffer, (_mode->resolution_y - 16
        - _mode->resolution_y % 16) * _mode->bytes_per_line);
    memory::zero((uint8_t *)_backbuffer + _mode->bytes_per_line * (_mode->resolution_y - 16 - _mode->resolution_y % 16), 
        _mode->bytes_per_line * 16);
    memory::copy((uint8_t *)_backbuffer, (uint8_t *)(uint64_t)_mode->addr, _mode->bytes_per_line * _mode->resolution_y);
    
    --_y;    
}

void screen::boot_terminal::set_color(color::colors c)
{
    uint32_t hex = c;
    
    _red = (hex >> 16) & 0xff;
    _green = (hex >> 8) & 0xff;
    _blue = hex & 0xff;
}
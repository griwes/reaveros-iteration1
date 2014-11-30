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

#include <screen/bootterm.h>
#include <memory/memory.h>
#include <memory/vm.h>

namespace screen
{
    utils::lazy<boot_terminal> term;
}

virt_addr_t _find_backbuffer(memory::map_entry * map, uint64_t map_size)
{
    for (uint64_t i = 0; i < map_size; ++i)
    {
        if (map[i].type == 4)
        {
            auto ret = memory::vm::allocate_address_range(map[i].length);
            memory::vm::map(virt_addr_t{ ret }, virt_addr_t{ ret } + map[i].length, phys_addr_t{ map[i].base });

            return ret;
        }
    }

    return {};
}

screen::boot_terminal::boot_terminal(screen::mode * mode, memory::map_entry * map, uint64_t map_size) : _mode{ mode },
    _backbuffer{ _find_backbuffer(map, map_size) }, _maxx{ static_cast<uint16_t>(mode->resolution_x / 8) },
    _maxy{ static_cast<uint16_t>(mode->resolution_y / 16) }, _x{}, _y{}
{
    _mode->addr = memory::vm::allocate_address_range(_mode->resolution_y * mode->bytes_per_line);
    memory::vm::map(_mode->addr, _mode->addr + mode->resolution_y * mode->bytes_per_line, phys_addr_t{ mode->short_addr });

    clear();

    set_color(color::gray);
}

screen::boot_terminal::~boot_terminal()
{
}

void screen::boot_terminal::clear()
{
    INTL();
    LOCK(_lock);

    if (_backbuffer)
    {
        memory::zero(static_cast<uint8_t *>(_backbuffer), _mode->resolution_y * _mode->bytes_per_line);
    }

    memory::zero(static_cast<uint8_t *>(_mode->addr), _mode->resolution_y * _mode->bytes_per_line);

    _x = 0;
    _y = 0;
}

void screen::boot_terminal::put_char(char c)
{
    INTL();
    LOCK(_lock);

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
    auto character = &(_mode->font[c * 16]);
    auto dest = static_cast<uint16_t *>(_mode->addr + _y * _mode->bytes_per_line * 16 + _x * 16);

    uint16_t * backdest;
    if (_backbuffer)
    {
        backdest = static_cast<uint16_t *>(_backbuffer + _y * _mode->bytes_per_line * 16 + _x * 16);
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

        auto _ = reinterpret_cast<uint64_t>(dest);
        _ += _mode->bytes_per_line;
        dest = reinterpret_cast<uint16_t *>(_);

        _ = reinterpret_cast<uint64_t>(backdest);
        _ += _mode->bytes_per_line;
        backdest = reinterpret_cast<uint16_t *>(_);
    }
}

void screen::boot_terminal::_put_32(char c)
{
    auto character = &(_mode->font[c * 16]);
    auto dest = static_cast<uint32_t *>(_mode->addr + _y * _mode->bytes_per_line * 16 + _x * 32);

    uint32_t * backdest;
    if (_backbuffer)
    {
        backdest = static_cast<uint32_t *>(_backbuffer + _y * _mode->bytes_per_line * 16 + _x * 32);
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

        auto _ = reinterpret_cast<uint64_t>(dest);
        _ += _mode->bytes_per_line;
        dest = reinterpret_cast<uint32_t *>(_);

        _ = reinterpret_cast<uint64_t>(backdest);
        _ += _mode->bytes_per_line;
        backdest = reinterpret_cast<uint32_t *>(_);
    }
}

void screen::boot_terminal::_scroll()
{
    memory::copy(static_cast<uint8_t *>(_backbuffer) + _mode->bytes_per_line * 16, static_cast<uint8_t *>(_backbuffer), (_mode->resolution_y - 16
        - _mode->resolution_y % 16) * _mode->bytes_per_line);
    memory::zero(static_cast<uint8_t *>(_backbuffer) + _mode->bytes_per_line * (_mode->resolution_y - 16 - _mode->resolution_y % 16),
        _mode->bytes_per_line * 16);
    memory::copy(static_cast<uint8_t *>(_backbuffer), static_cast<uint8_t *>(_mode->addr), _mode->bytes_per_line * _mode->resolution_y);

    --_y;
}

void screen::boot_terminal::set_color(color::colors c)
{
    INTL();
    LOCK(_lock);

    uint32_t hex = c;

    _red = (hex >> 16) & 0xff;
    _green = (hex >> 8) & 0xff;
    _blue = hex & 0xff;
}

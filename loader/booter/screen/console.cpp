#include "console.h"
#include "../memory/memory.h"

screen::console::console(screen::boot_mode * mode, void * font)
    : _mode(mode), _font((uint8_t *)font), _x(0), _y(0), _maxx(mode->resolution_x / 8), _maxy(mode->resolution_y / 16)
{
    _clear();
}

screen::console::~console()
{
}

void screen::console::put_char(char c)
{
    switch (c)
    {
        case '\0':
            return;
            
        case '\n':
            _x = 0;
            _y++;
            
            if (_y > _maxy)
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
    
    if (_x > _maxx)
    {
        _x = 0;
        _y++;
        
        if (_y > _maxy)
        {
            _scroll();
        }
    }
}

void screen::console::_put_16(char c)
{
    uint8_t * character = &(_font[c * 16]);
    uint16_t * dest = (uint16_t *)(_mode.addr + _y * _mode.bytes_per_line * 16 + _x * 16);
    
    uint16_t color = ((0xc0 >> (8 - _mode.red_size)) << _mode.red_pos) | ((0xc0 >> (8 - _mode.green_size)) 
        << _mode.green_pos) | ((0xc0 >> (8 - _mode.blue_size)) << _mode.blue_pos);
    
    for (uint64_t i = 0; i < 16; i++)
    {
        uint8_t data = character[i];
        
        for (uint64_t i = 0; i < 8; i++)
        {
            dest[i] = (data >> (7 - i)) & 1 ? color : 0;
        }
                
        uint32_t _ = (uint32_t)dest;
        _ += _mode.bytes_per_line;
        dest = (uint16_t *)_;
    }
}

void screen::console::_put_32(char c)
{
    uint8_t * character = &(_font[c * 16]);
    uint32_t * dest = (uint32_t *)(_mode.addr + _y * _mode.bytes_per_line * 16 + _x * 32);
    
    uint32_t color = ((0xc0 >> (8 - _mode.red_size)) << _mode.red_pos) | ((0xc0 >> (8 - _mode.green_size)) 
        << _mode.green_pos) | ((0xc0 >> (8 - _mode.blue_size)) << _mode.blue_pos);
    
    for (uint64_t i = 0; i < 16; i++)
    {
        uint8_t data = character[i];
        
        for (uint64_t i = 0; i < 8; i++)
        {
            dest[i] = (data >> (7 - i)) & 1 ? color : 0;
        }
        
        uint32_t _ = (uint32_t)dest;
        _ += _mode.bytes_per_line;
        dest = (uint32_t *)_;
    }
}

void screen::console::_clear()
{
    memory::zero((uint8_t *)_mode.addr, _mode.resolution_y * _mode.bytes_per_line);
}

void screen::console::_scroll()
{
    memory::copy((uint8_t *)_mode.addr + _mode.bytes_per_line, (uint8_t *)_mode.addr, (_mode.resolution_y - 1) 
        * _mode.bytes_per_line);
}

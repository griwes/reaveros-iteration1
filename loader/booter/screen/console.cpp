#include "console.h"

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
            
            return;
            
        case 32:
            _put_32(c);
            
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
            
            return;
    }
}

void screen::console::_put_16(char )
{

}

void screen::console::_put_32(char )
{

}

void screen::console::_clear()
{

}

void screen::console::_scroll()
{

}

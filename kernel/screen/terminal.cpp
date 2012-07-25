/**
 * ReaverOS
 * kernel/screen/terminal.cpp
 * Terminal implementation..
 */

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

#include "terminal.h"
#include "screen.h"
#include "../memory/memory.h"
#include "../memory/paging.h"
#include "../memory/vm.h"
#include "../memory/vmm.h"

Screen::Terminal::Terminal(Screen::TerminalDriver * pDrv, const Lib::String & sName)
            : m_sName(sName), m_pDriver(pDrv)
{
}

Screen::Terminal::~Terminal()
{
    delete m_pDriver;
}

void Screen::Terminal::ScrollUp()
{
    this->m_pDriver->ScrollUp();
}

void Screen::Terminal::ScrollDown()
{
    this->m_pDriver->ScrollDown();
}

Screen::ReaverTerminal::ReaverTerminal(Screen::TerminalDriver * pDrv, const Lib::String & sName)
        : Terminal(pDrv, sName)
{
}

Screen::ReaverTerminal::~ReaverTerminal()
{
}

void Screen::ReaverTerminal::Print(const Lib::String & )
{

}

void Screen::ReaverTerminal::Print(const char * )
{

}

Screen::BootTerminal::BootTerminal(Screen::VideoMode * pVideoMode, uint8 * pFont)
    : Terminal(0), x(0), y(0), maxx(pVideoMode->XResolution / 8), maxy(pVideoMode->YResolution / 16),
      m_pVideoMode(pVideoMode), m_pFont(pFont), r(0xbb), g(0xbb), b(0xbb), m_bBackbuffer(false)
{
    Memory::VMM::MapPages(Memory::VM::VideoMemoryBase, m_pVideoMode->YResolution *
        m_pVideoMode->BytesPerScanLine, m_pVideoMode->PhysBasePtr, true);
}

Screen::BootTerminal::~BootTerminal()
{
}

void Screen::BootTerminal::Print(const Lib::String & string)
{
    for (const char * i = string.Buffer(); *i != 0; i++)
    {
        this->_put_char(*i);
    }
}

void Screen::BootTerminal::Print(const char * string)
{
    for (; *string != 0; string++)
    {
        _put_char(*string);
    }
}

void Screen::BootTerminal::SetColor(Screen::Color color)
{
    switch (color)
    {
        case Black:
            this->r = this->g = this->b = 0x00;
            break;
        case Blue:
            this->r = this->g = 0x00;
            this->b = 0xbb;
            break;
        case Green:
            this->r = this->b = 0x00;
            this->g = 0xbb;
            break;
        case Cyan:
            this->r = 0x00;
            this->g = this->b = 0xbb;
            break;
        case Red:
            this->r = 0xbb;
            this->g = this->b = 0x00;
            break;
        case Magenta:
            this->r = this->b = 0xbb;
            this->g = 0x00;
            break;
        case Brown:
            this->r = this->g = 0xbb;
            this->b = 0x00;
            break;
        case Gray:
            this->r = this->g = this->b = 0xbb;
            break;
        case Charcoal:
            this->r = this->g = this->b = 0x55;
            break;
        case BrightBlue:
            this->r = this->g = 0x55;
            this->b = 0xff;
            break;
        case BrightGreen:
            this->r = this->b = 0x55;
            this->g = 0xff;
            break;
        case BrightCyan:
            this->r = 0x55;
            this->g = this->b = 0xff;
            break;
        case Orange:
            this->r = 0x55;
            this->g = this->b = 0xff;
            break;
        case Pink:
            this->r = this->b = 0xff;
            this->g = 0x55;
            break;
        case Yellow:
            this->r = this->g = 0xff;
            this->b = 0x55;
            break;
        case White:
            this->r = this->g = this->b = 0xff;
            break;
        default:
            this->r = this->g = this->b = 0xbb;
    }
}

void Screen::BootTerminal::_put_char(char c)
{
    if (c == 0)
    {
        return;
    }

    if (c == '\n')
    {
        this->x = 0;
        this->y++;

        if (this->y == this->maxy)
        {
            this->_scroll();
        }

        return;
    }

    if (c == '\r')
    {
        this->x = 0;
        return;
    }

    if (c == '\t')
    {
        this->x += (8 - this->x % 8);
        return;
    }

    // TODO:
/*    if (this->m_pVideoMode->XResolution == 0)
    {
        this->_put80x25(c);
        return;
    }*/

    switch (this->m_pVideoMode->BitsPerPixel)
    {
        case 16:
            this->_put16(c);
            break;
        case 32:
            this->_put32(c);
    }
}

void Screen::BootTerminal::_put16(char c)
{
    uint8 * character = &(this->m_pFont[c * 16]);
    uint16 * dest = (uint16 *)(Memory::VM::VideoMemoryBase + y * m_pVideoMode->BytesPerScanLine * 16 + x *
                    m_pVideoMode->BitsPerPixel);
    uint16 * dest_backbuffer = (uint16 *)(Memory::VM::VideoBackbufferBase + y * m_pVideoMode->BytesPerScanLine * 16
                    + x * m_pVideoMode->BitsPerPixel);
    
    uint16 iColor = ((this->r >> (8 - this->m_pVideoMode->RedMaskSize)) << this->m_pVideoMode->RedFieldPosition) |
                ((this->g >> (8 - this->m_pVideoMode->GreenMaskSize)) << this->m_pVideoMode->GreenFieldPosition) |
                ((this->b >> (8 - this->m_pVideoMode->BlueMaskSize)) << this->m_pVideoMode->BlueFieldPosition);
    
    uint16 iBgcolor = 0;
    
    for (int i = 0; i < 16; i++)
    {
        uint8 data = character[i];
        
        if (m_bBackbuffer)
        {
            for (uint8 i = 0; i < 8; i++)
            {
                dest_backbuffer[i] = (data >> (7 - i)) & 1 ? iColor : iBgcolor;
                dest[i] = dest_backbuffer[i];
            }
        }
        
        else
        {
            for (uint8 i = 0; i < 8; i++)
            {
                dest[i] = (data >> (7 - i)) & 1 ? iColor : iBgcolor;
            }
        }
        
        uint64 _ = (uint64)dest;
        _ += this->m_pVideoMode->BytesPerScanLine ;
        dest = (uint16 *)_;
    }
    
    this->x++;
    
    if (this->x == this->maxx)
    {
        this->x = 0;
        this->y++;

        if (this->y == this->maxy)
        {
            this->_scroll();
        }
    }
}

void Screen::BootTerminal::_put32(char c)
{
    uint8 * character = &(this->m_pFont[c * 16]);
    uint32 * dest = (uint32 *)(Memory::VM::VideoMemoryBase + y * m_pVideoMode->BytesPerScanLine * 16 + x *
                    m_pVideoMode->BitsPerPixel);
    uint32 * dest_backbuffer = (uint32 *)(Memory::VM::VideoBackbufferBase + y * m_pVideoMode->BytesPerScanLine * 16
                    + x * m_pVideoMode->BitsPerPixel);
    
    uint32 iColor = (this->r << this->m_pVideoMode->RedFieldPosition) |
                (this->g << this->m_pVideoMode->GreenFieldPosition) |
                (this->b << this->m_pVideoMode->BlueFieldPosition);
    
    uint32 iBgcolor = 0;
    
    for (int i = 0; i < 16; i++)
    {
        uint8 data = character[i];
        
        if (m_bBackbuffer)
        {        
            for (uint8 i = 0; i < 8; i++)
            {
                dest_backbuffer[i] = (data >> (7 - i)) & 1 ? iColor : iBgcolor;
                dest[i] = dest_backbuffer[i];
            }
        }
        
        else
        {
            for (uint8 i = 0; i < 8; i++)
            {
                dest[i] = (data >> (7 - i)) & 1 ? iColor : iBgcolor;
            }
        }
        
        uint64 _ = (uint64)dest;
        _ += this->m_pVideoMode->BytesPerScanLine;
        dest = (uint32 *)_;
    }
    
    this->x++;
    
    if (this->x == this->maxx)
    {
        this->x = 0;
        this->y++;

        if (this->y == this->maxy)
        {
            this->_scroll();
        }
    }
}

void Screen::BootTerminal::_scroll()
{
    if (m_bBackbuffer)
    {
        Memory::Copy((uint8 *)Memory::VM::VideoBackbufferBase + m_pVideoMode->BytesPerScanLine * 16,
                 (uint8 *)Memory::VM::VideoBackbufferBase, m_pVideoMode->BytesPerScanLine * (m_pVideoMode->YResolution - 16));
        Memory::Zero((uint8 *)Memory::VM::VideoBackbufferBase + m_pVideoMode->BytesPerScanLine * (m_pVideoMode->YResolution - 16),
                 m_pVideoMode->BytesPerScanLine * 16);
        Memory::Copy((uint8 *)Memory::VM::VideoBackbufferBase, (uint8 *)Memory::VM::VideoMemoryBase, m_pVideoMode->YResolution *
                 m_pVideoMode->BytesPerScanLine);
    }
    
    else
    {
        Memory::Copy((uint8 *)Memory::VM::VideoMemoryBase + m_pVideoMode->BytesPerScanLine * 16,
                 (uint8 *)Memory::VM::VideoMemoryBase, m_pVideoMode->BytesPerScanLine * (m_pVideoMode->YResolution - 16));
        Memory::Zero((uint8 *)Memory::VM::VideoMemoryBase + m_pVideoMode->BytesPerScanLine * (m_pVideoMode->YResolution - 16),
                 m_pVideoMode->BytesPerScanLine * 16);
    }
    
    --y;
}

void Screen::BootTerminal::Clear()
{
/*    if (m_pVideoMode->XResolution == 0)
    {
        _clear80x25();
    }

    else*/
    _clear();
}

void Screen::BootTerminal::_clear()
{
    Memory::Zero((uint8 *)Memory::VM::VideoMemoryBase, m_pVideoMode->BytesPerScanLine * m_pVideoMode->YResolution);
}

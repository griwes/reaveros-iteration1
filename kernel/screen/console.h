/**
 * ReaverOS
 * kernel/screen/console.h
 * Base console header.
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

#ifndef _rose_kernel_screen_console_h_
#define _rose_kernel_screen_console_h_

#include "../types.h"
#include "../lib/string.h"
#include "screen.h"

namespace Screen
{
    class Terminal;

    enum Color
    {
        Black,
        Blue,
        Green,
        Cyan,
        Red,
        Magenta,
        Brown,
        Gray,
        Charcoal,
        BrightBlue,
        BrightGreen,
        BrightCyan,
        Orange,
        Pink,
        Yellow,
        White
    };

    class Console
    {
    public:
        Console(Terminal *);
        virtual ~Console();
        
        enum Mode
        {
            Bin = 2,
            Oct = 8,
            Dec = 10,
            Hex = 16
        };

        Console & operator<<(char);
        Console & operator<<(const char *);
        Console & operator<<(const Lib::String &);
        Console & operator<<(Screen::Color);
        template<typename T> Console & operator<<(T);
        template<typename T> Console & operator<<(T *);

        uint64 GetBase()
        {
            return m_iBase;
        }
        
        void SetBase(uint64 newbase)
        {
            if (newbase > 32 || newbase < 2)
            {
                m_iBase = 36;
            }
            
            else
            {
                m_iBase = newbase;
            }
        }
        
        uint64 GetWidth()
        {
            return m_iWidth;
        }
        
        void SetWidth(uint64 newwidth)
        {
            m_iWidth = newwidth;
        }

        void HexNumbers(uint64 w, bool x0 = true)
        {
            m_iBase = 16;
            m_b16 = x0;
            m_i16Width = w;
        }

        void Clear();

    protected:
        uint64 m_iBase;
        uint64 m_iWidth;
        bool m_b16;
        uint64 m_i16Width;
        Terminal * m_pTerminal;
    };
}

template<typename T>
Screen::Console & Screen::Console::operator<<(T i)
{
    // first part for the sake of leaving warning away
    if (!(i > 0 || i == 0) && m_iBase == 10)
    {
        *this << '-';
        i = -i;
    }

    if (m_iBase == 16)
    {
        if (m_b16)
        {
            *this << "0x";
        }

        if (m_i16Width != 0)
        {
            for (int64 j = sizeof(T) / 16; j > 0; j--)
            {
                if ((i & (0xf << j)) != 0)
                    break;
                *this << '0';
            }
        }
        
        const char * digits = "0123456789ABCDEF";
        
        for (int64 j = sizeof(T) * 8 - 4; j >= 0; j -= 4)
        {
            *this << (digits[(i >> j) & 0xF]);
        }

        return *this;
    }

    if (m_iWidth != 0)
    {
        uint64 w = m_iWidth;
        for (uint64 j = i; w > 0 && j > 0; j /= m_iBase, w--) ;
        while (w-- > 0)
        {
            *this << '0';
        }
    }

    else if (i == 0)
    {
        *this << '0';
    }
    
    T div = i / m_iBase;
    T mod = i % m_iBase;
    
    if (div != 0)
    {
        *this << div;
    }
    
    *this << ("zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + mod]);
    
    return *this;
}

template<typename T>
Screen::Console & Screen::Console::operator<<(T * p)
{
    uint64 base = m_iBase;
    bool b = m_b16;
    uint64 w16 = m_i16Width;
    
    HexNumbers(16, true);

    *this << (uint64)p;

    m_iBase = base;
    m_b16 = b;
    m_i16Width = w16;

    return *this;
}

#endif
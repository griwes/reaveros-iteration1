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
        template<typename T> Console & operator<<(T);
        template<typename T> Console & operator<<(T *);

        uint64 GetBase()
        {
            return this->m_iBase;
        }
        
        void SetBase(uint64 newbase)
        {
            if (newbase > 36)
            {
                this->m_iBase = 36;
            }
            
            else
            {
                this->m_iBase = newbase;
            }
        }
        
        uint64 GetWidth()
        {
            return this->m_iWidth;
        }
        
        void SetWidth(uint64 newwidth)
        {
            this->m_iWidth = newwidth;
        }

        void HexNumbers(uint64 w, bool x0 = true)
        {
            this->m_iBase = 16;
            this->m_b16 = x0;
            this->m_i16Width = w;
        }

    protected:
        uint64 m_iBase;
        uint64 m_iWidth;
        bool m_b16;
        uint64 m_i16Width;
    };
}

#endif
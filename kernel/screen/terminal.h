/**
 * ReaverOS
 * kernel/screen/terminal.h
 * Terminal header.
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

#include "../lib/string.h"

namespace Screen
{
    class VideoMode;
    
    class TerminalDriver
    {
    public:
        TerminalDriver();
        virtual ~TerminalDriver() = 0;

        virtual void ScrollUp() = 0;
        virtual void ScrollDown() = 0;
    };

    class Terminal
    {
    public:
        Terminal(Screen::TerminalDriver *, const Lib::String & = "");
        virtual ~Terminal() = 0;

        virtual void Print(const Lib::String &) = 0;
        
        virtual void ScrollUp();
        virtual void ScrollDown();

        virtual Lib::String GetName()
        {
            return this->m_sName;
        }
        
        virtual void SetName(const Lib::String & s)
        {
            this->m_sName = s;
        }

    protected:
        Lib::String m_sName;
        Screen::TerminalDriver * m_pDriver;
    };

    class BootTerminal : public Terminal
    {
    public:
        BootTerminal(Screen::VideoMode *);
        virtual ~BootTerminal();

        virtual void Print(const Lib::String &);
    };

    class ReaverTerminal : public Terminal
    {
    public:
        ReaverTerminal(Screen::TerminalDriver *, const Lib::String & = "");
        virtual ~ReaverTerminal();

        virtual void Print(const Lib::String &);
    };
}
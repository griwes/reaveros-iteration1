/**
 * ReaverOS
 * kernel/screen/console.cpp
 * Base console implementation.
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

#include "console.h"
#include "terminal.h"

Screen::Console::Console(Screen::Terminal * pTerminal)
    : m_iBase(10), m_iWidth(0), m_b16(false), m_i16Width(0), m_pTerminal(pTerminal)
{
}

Screen::Console::~Console()
{
}

Screen::Console & Screen::Console::operator<<(char c)
{
    m_pTerminal->Print(c);
    return *this;
}

Screen::Console & Screen::Console::operator<<(const char * str)
{
    m_pTerminal->Print(str);
    return *this;
}

Screen::Console & Screen::Console::operator<<(const Lib::String & str)
{
    m_pTerminal->Print(str);
    return *this;
}

Screen::Console & Screen::Console::operator<<(Screen::Color color)
{
    m_pTerminal->SetColor(color);
    return *this;
}

void Screen::Console::Clear()
{
    m_pTerminal->Clear();
}

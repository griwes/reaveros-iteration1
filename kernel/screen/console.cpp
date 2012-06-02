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

Screen::Console::Mode Screen::Console::GetMode()
{

}

void Screen::Console::SetMode(Screen::Console::Mode )
{

}

void Screen::Console::Hex(uint64 , bool )
{

}

void Screen::Console::Dec(uint64 )
{

}

Screen::Console & Screen::Console::operator<<(char )
{
    return *this;
}

Screen::Console & Screen::Console::operator<<(const char * )
{
    return *this;
}

Screen::Console & Screen::Console::operator<<(const String & )
{
    return *this;
}

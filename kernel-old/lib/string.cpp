/**
 * ReaverOS
 * kernel/lib/string.cpp
 * Basic string implementation.
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

#include "string.h"
#include "../memory/memory.h"

Lib::String::String()
    : m_pData(0), m_iLength(0)
{
}

Lib::String::String(char c)
    : m_pData(new char[2]), m_iLength(1)
{
    m_pData[0] = c;
    m_pData[1] = 0;
}

Lib::String::String(const char * s)
    : m_pData(new char[Lib::Strlen(s) + 1]), m_iLength(Lib::Strlen(s) + 1)
{
    Memory::Copy(s, (const char *)m_pData, m_iLength);
}

Lib::String::String(const Lib::String & s)
    : m_pData(new char[s.m_iLength]), m_iLength(s.m_iLength)
{
    Memory::Copy(s.m_pData, m_pData, s.m_iLength);
}

Lib::String::~String()
{
    delete[] m_pData;
}

const char * Lib::String::Buffer() const
{
    return m_pData;
}

uint64 Lib::String::Length() const
{
    return m_iLength;
}

Lib::String Lib::String::operator+(const Lib::String & s)
{
    String temp;
    temp.m_iLength = m_iLength + s.m_iLength - 1;
    temp.m_pData = new char[temp.m_iLength];
    Memory::Copy(m_pData, temp.m_pData, m_iLength - 1);
    Memory::Copy(s.m_pData, temp.m_pData + m_iLength - 1, s.m_iLength);
    return temp;
}

Lib::String operator+(const char * s1, const Lib::String & s2)
{
    return Lib::String(s1) + s2;
}

Lib::String & Lib::String::operator+=(const Lib::String & s)
{
    *this = *this + s;
    return *this;
}

Lib::String & Lib::String::operator=(const Lib::String & s)
{
    delete[] m_pData;
    m_iLength = s.m_iLength;
    m_pData = new char[m_iLength];
    Memory::Copy(s.m_pData, m_pData, m_iLength);
    return *this;
}

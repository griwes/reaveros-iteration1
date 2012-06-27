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
    Memory::Copy(s, (const char *)this->m_pData, this->m_iLength);
}

Lib::String::String(const Lib::String & s)
    : m_pData(new char[s.m_iLength]), m_iLength(s.m_iLength)
{
    Memory::Copy(s.m_pData, this->m_pData, s.m_iLength);
}

Lib::String::~String()
{
    delete[] m_pData;
}

const char * Lib::String::Buffer() const
{
    return this->m_pData;
}

uint64 Lib::String::Length() const
{
    return this->m_iLength;
}

Lib::String Lib::String::operator+(const Lib::String & s)
{
    String temp;
    temp.m_iLength = this->m_iLength + s.m_iLength - 1;
    temp.m_pData = new char[temp.m_iLength];
    Memory::Copy(this->m_pData, temp.m_pData, this->m_iLength - 1);
    Memory::Copy(s.m_pData, temp.m_pData + this->m_iLength - 1, s.m_iLength);
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
    delete[] this->m_pData;
    this->m_iLength = s.m_iLength;
    this->m_pData = new char[this->m_iLength];
    Memory::Copy(s.m_pData, this->m_pData, this->m_iLength);
    return *this;
}

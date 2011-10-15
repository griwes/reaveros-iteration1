#include "screen.h"
#include "physmem.h"

extern __attribute__((cdecl)) void outb(unsigned short, unsigned char);
extern __attribute__((cdecl)) unsigned char inb(unsigned short);

void Screen::Initialize()
{
    Screen::kout = (Screen::Console *)PhysMemory::Manager::Place(sizeof(Screen::Console));
    Screen::kout->Initialize();
}

void Screen::Console::Initialize()
{
    Screen::Console::s_iMaxX = 80;
    Screen::Console::s_iMaxY = 25;

    Screen::Console::s_pScreenMemory = (char *)0x000b8000;
    
    this->m_iX = 0;
    this->m_iY = 0;
    
    this->m_iAttrib = 0x07;
}

Screen::Console & operator<<(Screen::Console & out, char iChar)
{
    if (iChar == '\n')
    {
        out.m_iY++;
        out.m_iX = 0;
        
        if (out.m_iY == Screen::Console::s_iMaxY)
        {
            out.Scroll();
        }
    }
    
    else if (iChar == '\t')
    {
        int i = out.m_iX % 8;
        if (i == 0)
            i = 8;
        
        while (i-- != 0)
        {
            out << ' ';
        }
    }
    
    else if (iChar == '\r')
    {
        out.m_iX = 0;
    }
    
    else if (iChar == '\b')
    {
        out.m_iX--;
        out << ' ';
    }
    
    else
    {
        char * pos = Screen::Console::s_pScreenMemory;
        pos += out.m
        *(Screen::Console::s_pScreenMemory) = iChar;
        *(Screen::Console::s_pScreenMemory + 1) = out.m_iAttrib;
        
        out.m_iX++;
        if (out.m_iX == Screen::Console::s_iMaxX)
        {
            out.m_iX = 0;
            out.m_iY++;
            
            if (out.m_iY == Screen::Console::s_iMaxY)
            {
                out.Scroll();
            }
        }
    }
    
    out.MoveCursor();
    
    return out;
}

Screen::Console & operator<<(Screen::Console & out, char * pString)
{
    while (pString != 0)
    {
        out << *pString;
        pString++;
    }
    
    return out;
}

Screen::Console & operator<<(Screen::Console & out, long int iInt)
{
    switch (out.m_eMode)
    {
        case Screen::ConsoleFlags::iBin:
            for (int i = 63; i >= 0; i--)
            {
                if (iInt & (1 << i))
                {
                    out << '1';
                }
                else
                {
                    out << '0';
                }
            }
            
            break;
        case Screen::ConsoleFlags::iOct:
            long int tmp, mod;
            
            tmp = iInt / 8;
            mod = iInt % 8;
            
            if (tmp > 0)
                out << tmp;
            
            out << (char)mod;
            
            break;
        case Screen::ConsoleFlags::iDec:
            long int tmp, mod;
            
            tmp = iInt / 10;
            mod = iInt % 10;
            
            if (tmp > 0)
                out << tmp;
            
            out << (char)mod;
            
            break;
        case Screen::ConsoleFlags::iHex:
            const char * digits = "0123456789ABCDEF";
            
            for (int i = 60; i > -1; i -= 4)
            {
                out << digits[(iInt >> i) & 0xF];
            }
    }
    
    return out;
}

Screen::Console & operator<<(Screen::Console & out, long int * pInt)
{
    return out << reinterpret_cast<long int>(pInt);
}

Screen::Console & operator<<(Screen::Console & out, int iInt)
{
    switch (out.m_eMode)
    {
        case Screen::ConsoleFlags::iBin:
            for (int i = 31; i >= 0; i--)
            {
                if (iInt & (1 << i))
                {
                    out << '1';
                }
                else
                {
                    out << '0';
                }
            }
            
            break;
        case Screen::ConsoleFlags::iOct:
            int tmp, mod;
            
            tmp = iInt / 8;
            mod = iInt % 8;
            
            if (tmp > 0)
                out << tmp;
            
            out << (char)mod;
            
            break;
        case Screen::ConsoleFlags::iDec:
            int tmp, mod;
            
            tmp = iInt / 10;
            mod = iInt % 10;
            
            if (tmp > 0)
                out << tmp;
            
            out << (char)mod;
            
            break;
        case Screen::ConsoleFlags::iHex:
            const char * digits = "0123456789ABCDEF";
            
            for (int i = 28; i > -1; i -= 4)
            {
                out << digits[(iInt >> i) & 0xF];
            }
    }
    
    return out;
}

Screen::Console & operator<<(Screen::Console & out, int * pInt)
{
    return out << reinterpret_cast<int>(pInt);
}

Screen::Console & operator<<(Screen::Console & out, double fDouble)
{
    // main double logic
}

Screen::Console & operator<<(Screen::Console & out, double * pDouble)
{
    return out << reinterpret_cast<long int>(pDouble);
}

Screen::Console & operator<<(Screen::Console & out, void * pPointer)
{
    return out << reinterpret_cast<long int>(pPointer);
}

Screen::Console & operator<<(Screen::Console & out, Screen::ConsoleFlags flag)
{
    out.m_eMode = flag;
    return out;
}

void Screen::Console::MoveCursor()
{
    
}

void Screen::Console::Scroll()
{
    char * target = Screen::Console::s_pScreenMemory;
    char * base = Screen::Console::s_pScreenMemory + Screen::Console::s_iMaxX * Screen::Console::s_iMaxY;
    
    for (int i = 0; i < Screen::Console::s_iMaxX * (Screen::Console::s_iMaxY - 1) * 2; i++)
    {
        *target = *base;
        
        target++;
        base++;
    }
}

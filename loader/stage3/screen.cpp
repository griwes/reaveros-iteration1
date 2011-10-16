#include "screen.h"
#include "physmem.h"

void outb(short port, char value)
{
    asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

char inb(short port)
{
    char ret;
    asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
    return ret;
}

namespace Screen
{
    Console * kout = 0;

    void Initialize()
    {
        Screen::kout = (Screen::Console *)PhysMemory::Manager::Place(sizeof(Screen::Console));
        Screen::kout->Initialize();
    }
}

void Screen::Console::Initialize()
{
    this->m_iMaxX = 80;
    this->m_iMaxY = 25;

    this->m_pScreenMemory = (volatile char *)0x000b8000;
    
    this->m_iX = 0;
    this->m_iY = 0;
    
    this->m_iAttrib = 0x07;
}

Screen::Console * Screen::Console::Print(char iChar)
{
    if (iChar == '\n')
    {
        this->m_iY++;
        this->m_iX = 0;
        
        if (this->m_iY == this->m_iMaxY)
        {
            this->Scroll();
        }
    }
    
    else if (iChar == '\t')
    {
        int i = this->m_iX % 8;
        if (i == 0)
            i = 8;
        
        while (i-- != 0)
        {
            this->Print(' ');
        }
    }
    
    else if (iChar == '\r')
    {
        this->m_iX = 0;
    }
    
    else if (iChar == '\b')
    {
        this->m_iX--;
        this->Print(' ');
        this->m_iX--;
    }
    
    else
    {
        volatile char * pos = this->m_pScreenMemory;
        pos += 2 * (this->m_iY * this->m_iMaxX + this->m_iX);
        *(pos) = iChar;
        *(pos + 1) = this->m_iAttrib;
        
        this->m_iX++;
        if (this->m_iX == this->m_iMaxX)
        {
            this->m_iX = 0;
            this->m_iY++;
            
            if (this->m_iY == this->m_iMaxY)
            {
                this->Scroll();
            }
        }
    }
    
    this->MoveCursor();
    
    return this;
}

Screen::Console * Screen::Console::Print(char * pString)
{
    char c;
    
    while (true)
    {
        c = *pString;
        if (c == 0)
            break;
        
        this->Print(c);
        pString++;
    }
    
    return this;
}

Screen::Console * Screen::Console::Print(long int iInt)
{
    long int tmp, mod;
    
    switch (this->m_eMode)
    {
        case iBin:
            for (int i = 63; i >= 0; i--)
            {
                if (iInt & (1 << i))
                {
                    this->Print('1');
                }
                else
                {
                    this->Print('0');
                }
            }
            
            break;
        case iOct:
            tmp = iInt / 8;
            mod = iInt % 8;
            
            if (tmp > 0)
                this->Print(tmp);
            
            this->Print((char)(mod + ('0' - 0)));
            
            break;
        case iDec:
            tmp = iInt / 10;
            mod = iInt % 10;
            
            if (tmp > 0)
                this->Print(tmp);
            
            this->Print((char)(mod + ('0' - 0)));
            
            break;
        case iHex:
            const char * digits = "0123456789ABCDEF";
            
            for (int i = 60; i > -1; i -= 4)
            {
                this->Print(digits[(iInt >> i) & 0xF]);
            }
    }
    
    return this;
}

Screen::Console * Screen::Console::Print(long int * pInt)
{
    return this->Print(reinterpret_cast<long int>(pInt));
}

Screen::Console * Screen::Console::Print(int iInt)
{
    int tmp, mod;
    
    switch (this->m_eMode)
    {
        case iBin:
            for (int i = 31; i >= 0; i--)
            {
                if (iInt & (1 << i))
                {
                    this->Print('1');
                }
                else
                {
                    this->Print('0');
                }
            }
            
            break;
        case iOct:
            tmp = iInt / 8;
            mod = iInt % 8;
            
            if (tmp > 0)
                this->Print(tmp);
            
            this->Print((char)(mod + ('0' - 0)));
            
            break;
        case iDec:
            tmp = iInt / 10;
            mod = iInt % 10;
            
            if (tmp > 0)
                this->Print(tmp);
            
            this->Print((char)(mod + ('0' - 0)));
            
            break;
        case iHex:
            const char * digits = "0123456789ABCDEF";
            
            for (int i = 28; i > -1; i -= 4)
            {
                this->Print(digits[(iInt >> i) & 0xF]);
            }
    }
    
    return this;
}

Screen::Console * Screen::Console::Print(int * pInt)
{
    return this->Print(reinterpret_cast<int>(pInt));
}

Screen::Console * Screen::Console::Print(double fDouble)
{
    return this;
    // main double logic
}

Screen::Console * Screen::Console::Print(double * pDouble)
{
    return this->Print(reinterpret_cast<long int>(pDouble));
}

Screen::Console * Screen::Console::Print(void * pPointer)
{
    return this->Print(reinterpret_cast<long int>(pPointer));
}

void Screen::Console::MoveCursor()
{
    asm("pusha");

    short i = this->m_iY * this->m_iMaxY + this->m_iX;
    
    outb(0x03d4, 0x0f);
    outb(0x03d5, i);
    outb(0x03d4, 0x0e);
    outb(0x03d5, i >> 8);
    
    asm("popa");
}

void Screen::Console::Scroll()
{
    volatile char * target = this->m_pScreenMemory;
    volatile char * base = this->m_pScreenMemory + this->m_iMaxX * this->m_iMaxY;
    
    for (int i = 0; i < this->m_iMaxX * (this->m_iMaxY - 1) * 2; i++)
    {
        *target = *base;
        
        target++;
        base++;
    }
}

void Screen::Console::Clear()
{
    volatile char * base = this->m_pScreenMemory;
    
    for (int i = 0; i < this->m_iMaxX * this->m_iMaxY; i++)
    {
        base[2 * i] = ' ';
        base[2 * i + 1] = Screen::Console::m_iAttrib;
    }
}

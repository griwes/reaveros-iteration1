#include "screen.h"
#include "physmem.h"

// helper functions
// in and out on a port
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
        // initialize kout
        // YES, I know that this call is not the prettiest one
        Screen::kout = (Screen::Console *)PhysMemory::Manager::Place(sizeof(Screen::Console));
        Screen::kout->Initialize();
    }
}

// setups default values for members
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
    // newline encountered
    if (iChar == '\n')
    {
        // go to new line
        this->m_iY++;
        this->m_iX = 0;
        
        // scroll if necessary
        if (this->m_iY == this->m_iMaxY)
        {
            this->Scroll();
        }
    }
    
    // tabulator
    else if (iChar == '\t')
    {
        // find how many spaces it is needed to fill the tabulator
        int i = (this->m_iX + 1) % 8;
        if (i == 0)
            i = 8;
        
        // and print them
        while (i-- != 0)
        {
            this->Print(' ');
        }
    }
    
    // carriage return; simplest possible
    else if (iChar == '\r')
    {
        this->m_iX = 0;
    }
    
    // backspace
    else if (iChar == '\b')
    {
        // overwrite the previous char
        this->m_iX--;
        this->Print(' ');
        // and set position at it
        this->m_iX--;
    }
    
    // actual printing
    else
    {
        // find where in memory the char should be written
        volatile char * pos = this->m_pScreenMemory;
        pos += 2 * (this->m_iY * this->m_iMaxX + this->m_iX);
        // write character 
        *(pos) = iChar;
        // and attribute
        *(pos + 1) = this->m_iAttrib;
        
        this->m_iX++;
        // end of line encountered
        if (this->m_iX == this->m_iMaxX)
        {
            // new line
            this->m_iX = 0;
            this->m_iY++;
            
            // scroll if necessary
            if (this->m_iY == this->m_iMaxY)
            {
                this->Scroll();
            }
        }
    }
    
    // update cursor position
    this->MoveCursor();
    
    return this;
}

Screen::Console * Screen::Console::Print(char * pString)
{
    char c;
    
    // print every character in string
    while (true)
    {
        c = *pString;
        // and stop at null
        if (c == 0)
            break;
        
        this->Print(c);
        pString++;
    }
    
    return this;
}

Screen::Console * Screen::Console::Print(unsigned long long int iInt)
{
    unsigned long long int tmp, mod;
    
    switch (this->m_eMode)
    {
        case iBin:
            // binary printing
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
            // octal printing
            tmp = iInt;
            mod = iInt % 8;
            
            if (tmp > 0)
                this->Print(tmp);
            
            this->Print((char)(mod + ('0' - 0)));
            
            break;
        case iDec:
            // decimal printing
            tmp = iInt / 10;
            mod = iInt % 10;
            
            if (tmp > 0)
                this->Print(tmp);
            
            this->Print((char)(mod + ('0' - 0)));
            
            break;
        case iHex:
            // hex printing
            const char * digits = "0123456789ABCDEF";
            
            for (int i = 60; i > -1; i -= 4)
            {
                this->Print(digits[(iInt >> i) & 0xF]);
            }
    }
    
    return this;
}

Screen::Console * Screen::Console::Print(unsigned long long int * pInt)
{
    return this->Print(reinterpret_cast<unsigned int>(pInt));
}

Screen::Console * Screen::Console::Print(unsigned int iInt)
{
    // see long int version
    unsigned int tmp, mod;
    
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

Screen::Console * Screen::Console::Print(unsigned int * pInt)
{
    return this->Print(reinterpret_cast<unsigned int>(pInt));
}

Screen::Console * Screen::Console::Print(double fDouble)
{
    return this;
    // main double logic
}

Screen::Console * Screen::Console::Print(double * pDouble)
{
    return this->Print(reinterpret_cast<unsigned int>(pDouble));
}

Screen::Console * Screen::Console::Print(void * pPointer)
{
    return this->Print(reinterpret_cast<unsigned int>(pPointer));
}

Screen::Console * Screen::Console::Print(const char * string)
{
    char c;
    
    while (true)
    {
        c = *string;
        
        if (c == 0)
            break;
        
        this->Print(c);
        string++;
    }
    
    return this;
}


void Screen::Console::MoveCursor()
{
    // move cursor; assembly magic
    asm("pusha");

    short i = this->m_iY * this->m_iMaxX + this->m_iX;
    
    outb(0x03d4, 0x0f);
    outb(0x03d5, i);
    outb(0x03d4, 0x0e);
    outb(0x03d5, i >> 8);
    
    asm("popa");
}

void Screen::Console::Scroll()
{
    volatile char * target = this->m_pScreenMemory;
    volatile char * base = this->m_pScreenMemory + this->m_iMaxX * 2;

    // move everything one line up
    for (int i = 0; i < this->m_iMaxX * (this->m_iMaxY - 1) * 2; i++)
    {
        *target = *base;
        
        target++;
        base++;
    }
    
    this->m_iX = 0;
    this->m_iY = this->m_iMaxY - 1;
    
    // print spaces at last line
    for (int i = 0; i < this->m_iMaxX; i++)
    {
        this->Print(' ');
    }
    
    this->m_iY = this->m_iMaxY - 1;
    this->m_iX = 0;
}

void Screen::Console::Clear()
{
    volatile char * base = this->m_pScreenMemory;
    
    // clear
    for (int i = 0; i < this->m_iMaxX * this->m_iMaxY; i++)
    {
        base[2 * i] = ' ';
        base[2 * i + 1] = Screen::Console::m_iAttrib;
    }
    
    // gotoxy(0, 0);
    this->m_iX = 0;
    this->m_iY = 0;
    this->MoveCursor();
}

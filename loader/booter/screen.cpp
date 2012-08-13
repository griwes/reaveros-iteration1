/**
 * ReaverOS
 * loader/booter/screen.h
 * Screen drawing routines.
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

#include "screen.h"
#include "memory.h"
#include "paging.h"
#include "processor.h"

extern "C"
{
    void _reload_cr3(uint32);
}

namespace Screen
{
    OutputStream * bout = 0;
    const char * nl = "\n";
    const char * tab = "\t";
}

void Screen::Initialize(VideoMode * pVideoMode, void * pFont)
{
    VideoModeWrapper * pVideoModeWrapper = (VideoModeWrapper *)Memory::Place(sizeof(VideoModeWrapper));
    pVideoModeWrapper->Initialize(pFont, pVideoMode);
    Screen::bout = (OutputStream *)Memory::Place(sizeof(OutputStream));
    Screen::bout->Initialize(pVideoModeWrapper);
}

uint8 OutputStream::Base(uint8 base)
{
    if (!(base < 2 || base > 32))
    {
        m_iBase = base;
    }
    
    return m_iBase;
}

void OutputStream::Initialize(VideoModeWrapper * pVideoMode)
{
    m_iBase = 10;
    m_pVideoMode = pVideoMode;
}

uint64 Screen::SaveProcessedVideoModeDescription(uint64 pDestAddress)
{
    uint64 p = Memory::iFirstFreePageAddress;
    Memory::Map(pDestAddress, pDestAddress + 4096, Memory::iFirstFreePageAddress);
    Memory::Map(0x8000000, 0x8000000 + 4096, p);

    KernelVideoMode * kvideo = (KernelVideoMode *)0x8000000;

    kvideo->PhysBasePtr = Screen::bout->m_pVideoMode->m_pVideoMode->PhysBasePtr;
    kvideo->BytesPerScanLine = Screen::bout->m_pVideoMode->m_pVideoMode->LinearBytesPerScanLine;
    kvideo->XResolution = Screen::bout->m_pVideoMode->m_pVideoMode->XResolution;
    kvideo->YResolution = Screen::bout->m_pVideoMode->m_pVideoMode->YResolution;
    kvideo->BitsPerPixel = Screen::bout->m_pVideoMode->m_pVideoMode->BitsPerPixel;
    kvideo->RedMaskSize = Screen::bout->m_pVideoMode->m_pVideoMode->RedMaskSize;
    kvideo->RedFieldPosition = Screen::bout->m_pVideoMode->m_pVideoMode->RedFieldPosition;
    kvideo->GreenMaskSize = Screen::bout->m_pVideoMode->m_pVideoMode->GreenMaskSize;
    kvideo->GreenFieldPosition = Screen::bout->m_pVideoMode->m_pVideoMode->GreenFieldPosition;
    kvideo->BlueMaskSize = Screen::bout->m_pVideoMode->m_pVideoMode->BlueMaskSize;
    kvideo->BlueFieldPosition = Screen::bout->m_pVideoMode->m_pVideoMode->BlueFieldPosition;
    kvideo->ReservedMaskSize = Screen::bout->m_pVideoMode->m_pVideoMode->RsvdMaskSize;
    kvideo->ReservedFieldPosition = Screen::bout->m_pVideoMode->m_pVideoMode->RsvdFieldPosition;

    return pDestAddress + 4096;
}

void VideoModeWrapper::Initialize(void * pFont, VideoMode * pVideoMode)
{
    m_pVideoMode = pVideoMode;

    if (m_pVideoMode->LinearBlueFieldPosition == 0 && m_pVideoMode->LinearGreenFieldPosition == 0
            && m_pVideoMode->LinearRedFieldPosition == 0 && m_pVideoMode->LinearBytesPerScanLine == 0)
    {
        m_pVideoMode->LinearRedFieldPosition = m_pVideoMode->RedFieldPosition;
        m_pVideoMode->LinearGreenFieldPosition = m_pVideoMode->GreenFieldPosition;
        m_pVideoMode->LinearBlueFieldPosition = m_pVideoMode->BlueFieldPosition;
        m_pVideoMode->LinearRedMaskSize = m_pVideoMode->RedMaskSize;
        m_pVideoMode->LinearGreenMaskSize = m_pVideoMode->GreenMaskSize;
        m_pVideoMode->LinearBlueMaskSize = m_pVideoMode->BlueMaskSize;
        m_pVideoMode->LinearBytesPerScanLine = m_pVideoMode->BytesPerScanLine;
    }

    maxx = m_pVideoMode->XResolution / 8;
    maxy = m_pVideoMode->YResolution / 16;

    x = 0;
    y = 0;

    r = g = b = 0xc0;
    _ = 0;

    m_pFontData = pFont;
}

OutputStream & operator << (OutputStream & s, const char * str)
{
    while (*str)
    {
        s.m_pVideoMode->PrintCharacter(*str++);
    }

    return s;
}

void VideoModeWrapper::PrintCharacter(char c)
{
    if (c == 0)
    {
        return;
    }

    if (c == '\n')
    {
        y++;
        x = 0;
        return;
    }

    if (c == '\t')
    {
        x += (8 - x % 8);
        return;
    }

    switch (m_pVideoMode->BitsPerPixel)
    {
        case 16:
        {
            _put16(c);
            return;
        }

        case 32:
        {
            _put32(c);
            return;
        }
    }
}

void VideoModeWrapper::_put16(char c)
{
    uint8 * character = &((uint8 *)m_pFontData)[c * 16];
    volatile uint16 * dest = (uint16 *)(m_pVideoMode->PhysBasePtr + y * m_pVideoMode->LinearBytesPerScanLine * 16
                    + x * m_pVideoMode->BitsPerPixel);

    uint16 iColor = ((r >> (8 - m_pVideoMode->LinearRedMaskSize)) << m_pVideoMode->LinearRedFieldPosition) |
                ((g >> (8 - m_pVideoMode->LinearGreenMaskSize)) << m_pVideoMode->LinearGreenFieldPosition) |
                ((b >> (8 - m_pVideoMode->LinearBlueMaskSize)) << m_pVideoMode->LinearBlueFieldPosition);

    uint16 iBgcolor = 0;
                    
    for (int i = 0; i < 16; i++)
    {
        uint8 data = character[i];

        for (uint8 i = 0; i < 8; i++)
        {
            dest[i] = (data >> (7 - i)) & 1 ? iColor : iBgcolor;
        }

        uint32 _ = (uint32)dest;
        _ += m_pVideoMode->BytesPerScanLine ;
        dest = (uint16 *)_;
    }

    x++;

    if (x > maxx)
    {
        x = 0;
        y++;
    }
}

void VideoModeWrapper::_put32(char c)
{
    uint8 * character = &((uint8 *)m_pFontData)[c * 16];
    volatile uint32 * dest = (uint32 *)(m_pVideoMode->PhysBasePtr + y * m_pVideoMode->LinearBytesPerScanLine * 16
                    + x * m_pVideoMode->BitsPerPixel);

    uint32 iColor = (r << m_pVideoMode->LinearRedFieldPosition) |
                    (g << m_pVideoMode->LinearGreenFieldPosition) |
                    (b << m_pVideoMode->LinearBlueFieldPosition);

    uint32 iBgcolor = 0;
    
    for (int i = 0; i < 16; i++)
    {
        uint8 data = character[i];

        for (uint8 i = 0; i < 8; i++)
        {
            dest[i] = (data >> (7 - i)) & 1 ? iColor : iBgcolor;
        }

        uint32 _ = (uint32)dest;
        _ += m_pVideoMode->BytesPerScanLine;
        dest = (uint32 *)_;
    }

    x++;
    
    if (x > maxx)
    {
        x = 0;
        y++;
    }
}

void OutputStream::UpdatePagingStructures()
{    
    uint64 vidmem = m_pVideoMode->m_pVideoMode->PhysBasePtr;
    uint64 vidmemsize = m_pVideoMode->m_pVideoMode->YResolution *
            m_pVideoMode->m_pVideoMode->LinearBytesPerScanLine;

    Memory::Map(vidmem, vidmem + vidmemsize, vidmem, true);
}

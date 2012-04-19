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

#ifndef __rose_loader_booter_screen_h__
#define __rose_loader_booter_screen_h__

#include "types.h"

class VideoMode
{
public:
    uint16 ModeAttributes;
    uint8 WindowAAttributes;
    uint8 WindowBAttributes;
    uint16 WindowGranularity;
    uint16 WindowSize;
    uint16 WindowASegment;
    uint16 WindowBSegment;
    uint16 WindowFunctionPointerOffset;
    uint16 WindowFunctionPointerSegment;
    uint16 BytesPerScanLine;

    uint16 XResolution;
    uint16 YResolution;
    uint8 XCharSize;
    uint8 YCharSize;
    uint8 NumberOfPlanes;
    uint8 BitsPerPixel;
    uint8 NumberOfBanks;
    uint8 MemoryModel;
    uint8 BankSize;
    uint8 NumberOfImagePages;
    uint8 _;
    uint8 RedMaskSize, RedFieldPosition;
    uint8 GreenMaskSize, GreenFieldPosition;
    uint8 BlueMaskSize, BlueFieldPosition;
    uint8 RsvdMaskSize, RsvdFieldPosition;
    uint8 DirectColorModeInfo;

    uint32 PhysBasePtr;
    uint32 __;
    uint16 ___;

    uint16 LinearBytesPerScanLine;
    uint8 BankNumberOfImagePages;
    uint8 LinearNumberOfImagePages;
    uint8 LinearRedMaskSize, LinearRedFieldPosition;
    uint8 LinearGreenMaskSize, LinearGreenFieldPosition;
    uint8 LinearBlueMaskSize, LinearBlueFieldPosition;
    uint8 LinearRsvdMaskSize, LinearRsvdFieldPosition;
    uint32 MaximumPixelClock;

    uint8 ____[189];
} __attribute__((packed));

class VideoModeWrapper
{
public:
    friend class OutputStream;
    
    void Initialize(void *, VideoMode *);
    void PrintCharacter(char);
    void ClearScreen();
    
private:
    VideoMode * m_pVideoMode;
    uint8 x, y;
    uint8 maxx, maxy;

    void _put16(char);
    void _put32(char);

    uint8 r, g, b, _;

    void * m_pFontData;
} __attribute__((packed));

class VideoModeDescription
{
public:
    uint16 ModeAttributes;

    uint64 PhysicalVideoMemoryAddress;
    uint16 LineSize;
    
    uint16 XResolution;
    uint16 YResolution;
    
    uint8 RedSize, RedPosition;
    uint8 GreenSize, GreenPosition;
    uint8 BlueSize, BluePosition;
    uint8 RsvdSize, RsvdPosition;
} __attribute__((packed));

class OutputStream
{
public:
    void Initialize(VideoModeWrapper *);
    friend OutputStream & operator << (OutputStream &, const char *);
    template<typename T>
    friend OutputStream & operator << (OutputStream &, T);

    void Hex()
    {
        this->Base(16);
    }
    
    void Dec()
    {
        this->Base(10);
    }
    
    uint8 Base(uint8 = 0);

    void UpdatePagingStructures();
private:
    uint8 m_iBase;
    VideoModeWrapper * m_pVideoMode;
};

namespace Screen
{
    void Initialize(VideoMode *, void * pFont);
    uint64 SaveProcessedVideoModeDescription(uint64);

    extern OutputStream * bout;
    extern const char * nl;
    extern const char * tab;
}

template<typename T>
OutputStream & operator << (OutputStream &s, T i)
{
    if (i < 0 && s.m_iBase == 10)
    {
        s.m_pVideoMode->PrintCharacter('-');
        i = -i;
    }
    
    if (i == 0)
    {
        s.m_pVideoMode->PrintCharacter(0);
    }
    
    T div = i / s.m_iBase;
    T mod = i % s.m_iBase;
    
    if (div != 0)
    {
        s << div;
    }
    
    s.m_pVideoMode->PrintCharacter("zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + mod]);
    
    return s;
}

#endif

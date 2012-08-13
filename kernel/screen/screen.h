/**
 * ReaverOS
 * kernel/screen/screen.h
 * Main display header.
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

#ifndef _rose_kernel_screen_screen_h_
#define _rose_kernel_screen_screen_h_

#include "../types.h"
#include "console.h"

namespace Screen
{
    class Console;
    
    extern Screen::Console * kout;

    struct VideoMode
    {
        uint32 PhysBasePtr;
        uint16 BytesPerScanLine;
        uint16 XResolution, YResolution;
        uint8 BitsPerPixel;
        uint16 RedMaskSize, RedFieldPosition;
        uint16 GreenMaskSize, GreenFieldPosition;
        uint16 BlueMaskSize, BlueFieldPosition;
        uint16 ReservedMaskSize, ReservedFieldPosition;
    } __attribute__((__packed__));

    extern char nl;
    extern Screen::VideoMode * Mode;
    
    void Initialize(Screen::VideoMode *, uint8 *);
    void PrintStatus();
}

#endif

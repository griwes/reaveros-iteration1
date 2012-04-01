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

uint32 MaskTable[16] = {
    0x00000000,
    0x000000FF,
    0x0000FF00,
    0x0000FFFF,
    0x00FF0000,
    0x00FF00FF,
    0x00FFFF00,
    0x00FFFFFF,
    0xFF000000,
    0xFF0000FF,
    0xFF00FF00,
    0xFF00FFFF,
    0xFFFF0000,
    0xFFFF00FF,
    0xFFFFFF00,
    0xFFFFFFFF
};

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
        this->m_iBase = base;
    }
    
    return this->m_iBase;
}

void OutputStream::Initialize(VideoModeWrapper * pVideoMode)
{
    this->m_iBase = 10;
    this->m_pVideoMode = pVideoMode;
}

VideoModeDescription * Screen::GetProcessedVideoModeDescription()
{
    VideoModeDescription * ret = (VideoModeDescription *)Memory::Place(sizeof(VideoModeDescription));

    return ret;
}

void VideoModeWrapper::Initialize(void * pFont, VideoMode * pVideoMode)
{
    this->m_pVideoMode = pVideoMode;

    if (this->m_pVideoMode->LinearBlueFieldPosition == 0 && this->m_pVideoMode->LinearGreenFieldPosition == 0
            && this->m_pVideoMode->LinearRedFieldPosition == 0 && this->m_pVideoMode->LinearBytesPerScanLine == 0)
    {
        this->m_pVideoMode->LinearRedFieldPosition = this->m_pVideoMode->RedFieldPosition;
        this->m_pVideoMode->LinearGreenFieldPosition = this->m_pVideoMode->GreenFieldPosition;
        this->m_pVideoMode->LinearBlueFieldPosition = this->m_pVideoMode->BlueFieldPosition;
        this->m_pVideoMode->LinearRedMaskSize = this->m_pVideoMode->RedMaskSize;
        this->m_pVideoMode->LinearGreenMaskSize = this->m_pVideoMode->GreenMaskSize;
        this->m_pVideoMode->LinearBlueMaskSize = this->m_pVideoMode->BlueMaskSize;
        this->m_pVideoMode->LinearBytesPerScanLine = this->m_pVideoMode->BytesPerScanLine;
    }

    this->maxx = this->m_pVideoMode->XResolution / 8;
    this->maxy = this->m_pVideoMode->YResolution / 16;

    this->x = 0;
    this->y = 0;

    this->r = this->g = this->b = 0xc0;
    this->_ = 0;

    this->m_pFontData = pFont;
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
        this->y++;
        this->x = 0;
        return;
    }

    if (c == '\t')
    {
        this->x += (8 - this->x % 8);
        return;
    }

    switch (this->m_pVideoMode->BitsPerPixel)
    {
        case 16:
        {
            this->_put16(c);
            return;
        }

        case 32:
        {
            this->_put32(c);
            return;
        }
    }
}

void VideoModeWrapper::_put16(char c)
{
    uint8 * character = &((uint8 *)this->m_pFontData)[c * 16];
    uint32 * dest = (uint32 *)(this->m_pVideoMode->PhysBasePtr + this->y * this->m_pVideoMode->LinearBytesPerScanLine * 16
                    + this->x * this->m_pVideoMode->BitsPerPixel);

    uint16 iColor = ((this->r >> (8 - this->m_pVideoMode->LinearRedMaskSize)) << this->m_pVideoMode->LinearRedFieldPosition) |
                ((this->g >> (8 - this->m_pVideoMode->LinearGreenMaskSize)) << this->m_pVideoMode->LinearGreenFieldPosition) |
                ((this->b >> (8 - this->m_pVideoMode->LinearBlueMaskSize)) << this->m_pVideoMode->LinearBlueFieldPosition);

    uint32 iPackedColor = iColor | iColor << 16;

    uint32 iBgcolor = 0;
                    
    for (int i = 0; i < 16; i++)
    {
        uint8 data = character[i];
        uint32 mask1, mask2;
        
        mask1 = MaskTable[(data & 0xf0) >> 4];
        mask2 = MaskTable[data & 0xf];
        
        uint32 mask16[4];
        
        uint16 * mask161, * mask162;
        mask161 = (uint16 *)(&mask1);
        mask162 = (uint16 *)(&mask2);
        
        mask16[0] = MaskTable[(mask161[1] >> 4) & 0xf];
        mask16[1] = MaskTable[mask161[0] & 0xf];
        mask16[2] = MaskTable[(mask162[1] >> 4) & 0xf];
        mask16[3] = MaskTable[mask162[0] & 0xf];

        dest[0] = (iPackedColor & mask16[0]) | (iBgcolor & ~mask16[0]);
        dest[1] = (iPackedColor & mask16[1]) | (iBgcolor & ~mask16[1]);
        dest[2] = (iPackedColor & mask16[2]) | (iBgcolor & ~mask16[2]);
        dest[3] = (iPackedColor & mask16[3]) | (iBgcolor & ~mask16[3]);

        uint32 _ = (uint32)dest;
        _ += this->m_pVideoMode->BytesPerScanLine;
        dest = (uint32 *)_;
    }

    this->x++;

    if (this->x > this->maxx)
    {
        this->x = 0;
        this->y++;
    }
}

void VideoModeWrapper::_put32(char c)
{
    uint8 * character = &((uint8 *)this->m_pFontData)[c * 16];
    uint32 * dest = (uint32 *)(this->m_pVideoMode->PhysBasePtr + this->y * this->m_pVideoMode->LinearBytesPerScanLine * 16
                    + this->x * this->m_pVideoMode->BitsPerPixel);

    uint32 iColor = (this->r << this->m_pVideoMode->LinearRedFieldPosition) |
                    (this->g << this->m_pVideoMode->LinearGreenFieldPosition) |
                    (this->b << this->m_pVideoMode->LinearBlueFieldPosition);

    uint32 iBgcolor = 0;
    
    for (int i = 0; i < 16; i++)
    {
        uint8 data = character[i];

        uint32 mask1, mask2;
        mask1 = MaskTable[(data & 0xf0) >> 4];
        mask2 = MaskTable[data & 0xf];
        
        dest[0] = ((mask1 >> 28) & 0xf) ? iColor : iBgcolor;
        dest[1] = ((mask1 >> 20) & 0xf) ? iColor : iBgcolor;
        dest[2] = ((mask1 >> 12) & 0xf) ? iColor : iBgcolor;
        dest[3] = (mask1 & 0xf) ? iColor : iBgcolor;
        dest[4] = ((mask2 >> 28) & 0xf) ? iColor : iBgcolor;
        dest[5] = ((mask2 >> 20) & 0xf) ? iColor : iBgcolor;
        dest[6] = ((mask2 >> 12) & 0xf) ? iColor : iBgcolor;
        dest[7] = (mask2 & 0xf) ? iColor : iBgcolor;
        
        uint32 _ = (uint32)dest;
        _ += this->m_pVideoMode->BytesPerScanLine;
        dest = (uint32 *)_;
    }

    this->x++;
    
    if (this->x > this->maxx)
    {
        this->x = 0;
        this->y++;
    }
}

/**
 * I know, the following method is huge.
 * I tried to split it, but I hate functions taking 9 parameters
 * more than I hate huge functions. So here it is.
 *
 * Basically, it identity maps the video memory. There are checks
 * to ensure that we are not deleting something from paging structures,
 * I think that it's pretty straightforward to find out what happens
 * here, so sorry for no comments in body of function.
 */
void OutputStream::UpdatePagingStructures()
{
    PML4 * pml4 = Processor::PagingStructures;

    uint32 vidmem = this->m_pVideoMode->m_pVideoMode->PhysBasePtr;
    uint32 vidmemsize = this->m_pVideoMode->m_pVideoMode->YResolution *
            this->m_pVideoMode->m_pVideoMode->LinearBytesPerScanLine;

    uint32 vidmemstartpaged = vidmem >> 12;
    uint32 vidmemendpaged = (vidmem + vidmemsize) >> 12;

    uint32 startpml4e = vidmemstartpaged / (512 * 512 * 512);
    uint32 startpdpte = (vidmemstartpaged % (512 * 512 * 512)) / (512 * 512);
    uint32 startpde = (vidmemstartpaged % (512 * 512)) / 512;
    uint32 startpte = vidmemstartpaged % 512;

    uint32 endpml4e = vidmemendpaged / (512 * 512 * 512);
    uint32 endpdpte = (vidmemendpaged % (512 * 512 * 512)) / (512 * 512);
    uint32 endpde = (vidmemendpaged % (512 * 512)) / 512;
    uint32 endpte = vidmemendpaged % 512;
    
    while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte))
    {
        PageDirectoryPointerTable * pdpt;

        if (pml4->Entries[startpml4e].Present == 1)
        {
            pml4->Entries[startpml4e].CacheDisable = 1;
            pdpt = pml4->PointerTables[startpml4e];
        }

        else
        {
            pdpt = (PageDirectoryPointerTable *)Memory::PlacePageAligned(
                                                sizeof(PageDirectoryPointerTable));

            Memory::Zero((char *)pdpt, sizeof(PageDirectoryPointerTable));
            
            pml4->Entries[startpml4e].Present = 1;
            pml4->Entries[startpml4e].ReadWrite = 1;
            pml4->Entries[startpml4e].CacheDisable = 1;
            pml4->Entries[startpml4e].PDPTAddress = (uint32)pdpt >> 12;
            
            pml4->PointerTables[startpml4e] = pdpt;
        }

        while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
                && startpdpte < 512)
        {
            PageDirectory * pd;
            
            if (pdpt->Entries[startpdpte].Present == 1)
            {
                pdpt->Entries[startpdpte].CacheDisable = 1;
                pd = pdpt->PageDirectories[startpdpte];
            }

            else
            {
                pd = (PageDirectory *)Memory::PlacePageAligned(sizeof(PageDirectory));

                Memory::Zero((char *)pd, sizeof(PageDirectory));

                pdpt->Entries[startpdpte].Present = 1;
                pdpt->Entries[startpdpte].ReadWrite = 1;
                pdpt->Entries[startpdpte].CacheDisable = 1;
                pdpt->Entries[startpdpte].PageDirectoryAddress = (uint32)pd >> 12;

                pdpt->PageDirectories[startpdpte] = pd;
            }
            
            while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
                    && startpde < 512)
            {
                PageTable * pt;
                
                if (pd->Entries[startpde].Present == 1)
                {
                    pd->Entries[startpde].CacheDisable = 1;
                    pt = pd->PageTables[startpde];
                }

                else
                {
                    pt = (PageTable *)Memory::PlacePageAligned(sizeof(PageTable));

                    Memory::Zero((char *)pt, sizeof(PageTable));

                    pd->Entries[startpde].Present = 1;
                    pd->Entries[startpde].ReadWrite = 1;
                    pd->Entries[startpde].CacheDisable = 1;
                    pd->Entries[startpde].PageTableAddress = (uint32)pt >> 12;

                    pd->PageTables[startpde] = pt;
                }

                while (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte)
                        && startpte < 512)
                {
                    pt->Entries[startpte].Present = 1;
                    pt->Entries[startpte].ReadWrite = 1;
                    pt->Entries[startpte].CacheDisable = 1;

                    uint64 addr = 512ull * 1024 * 1024 * 1024 * startpml4e + 1024ull * 1024 * 1024 * startpdpte +
                                2ull * 1024 * 1024 * startpde + 4ull * 1024 * startpte;
                    
                    pt->Entries[startpte].PageAddress = addr >> 12;

                    startpte++;
                }

                if (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte))
                {
                    startpde++;
                    startpte = 0;
                }

                else
                {
                    return;
                }
            }
            
            if (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte))
            {
                startpde = 0;
                startpdpte++;
            }

            else
            {
                return;
            }
        }

        if (!(startpml4e == endpml4e && startpdpte == endpdpte && startpde == endpde && startpte == endpte))
        {
            startpdpte = 0;
            startpml4e++;
        }

        else
        {
            return;
        }
    }
}

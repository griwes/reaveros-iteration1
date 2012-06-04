/**
 * ReaverOS
 * loader/booter/main.cpp
 * Booter main file.
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

#include "types.h"
#include "memory.h"
#include "screen.h"
#include "processor.h"

using Screen::bout;
using Screen::nl;

extern "C" void booter_main(MemoryMapEntry * pMemoryMap, uint32 iMemoryMapSize, void * pPlacementAddress,
                            uint32 pKernel, uint32 pKernelSize, uint32 pKernelInitRDSize, VideoMode * pVideoMode,
                            void * pFont)
{
    Memory::Initialize(pPlacementAddress);
    Screen::Initialize(pVideoMode, pFont);

    *bout << "Booter: ReaverOS' bootloader 0.2" << nl;
    *bout << "Copyright (c) 2011-2012 Reaver Project Team" << nl << nl;
    *bout << "Reading memory map..." << nl << nl;

    Memory::PrintMemoryMap(pMemoryMap, iMemoryMapSize);
    
    *bout << "Entering long mode...";

    Processor::EnterLongMode();

    *bout << " done." << nl;

    Processor::SetupGDT();

    // copy kernel and initrd, put video mode description after it
    // emplace (not finished) memory map back there

    // algorithm for this:
    // 1. map enough pages to make a copy at both destination address and address in low memory
    // 2. copy data to those pages sitting in low memory of VAS

    *bout << "Preparing kernel memory...";
    
    uint64 end = Memory::Copy(pKernel, pKernelSize * 512, 0xFFFFFFFF80000000); // -2 GB
    uint64 videofont = Memory::Copy(pKernel + pKernelSize * 512, pKernelInitRDSize * 512, Memory::AlignToNextPage(end));
    uint64 video = Memory::Copy((uint32)pFont, 4096, Memory::AlignToNextPage(videofont));
    uint64 memmap = Screen::SaveProcessedVideoModeDescription(video);
    uint64 placement = Memory::CreateMemoryMap(pMemoryMap, iMemoryMapSize, memmap);
    
    // magic call. maps memory from kernel start.
    // amount of memory to map should be enough for kernel to recreate paging structures
    // in it's own, completely known space (part of boot protocol), as well as additional 16 MiB
    // for additional stuff to be put on placement stack

    uint64 size = Memory::CountPagingStructures(0xFFFFFFFF80000000, Memory::AlignToNextPage(placement)
                                + 16 * 1024 * 1024 + Memory::TotalMemory / 512);
    size += Memory::AlignToNextPage(16 * 1024 * 1024 + Memory::TotalMemory / 512);
    
    Memory::Map(placement, placement + size, Memory::iFirstFreePageAddress);
    
    // and this one updates memory map, setting size to type 0xFFFF entry (kernel-used memory)
    // that starts at 64 MiB in physical memory
    
    Memory::UpdateMemoryMap(placement - 0xFFFFFFFF80000000 + size);

    *bout << " done." << nl;
    bout->Hex();
    *bout << "Memory from " << 0xFFFFFFFF80000000 << " to " << placement + size << " available for kernel usage." << nl;
    *bout << "Executing kernel...";

    Processor::Execute(pKernel, Memory::AlignToNextPage(end), memmap, iMemoryMapSize + 1,
                       Memory::AlignToNextPage(placement), video, videofont);
    
    for (;;) ;
    
    return;
}
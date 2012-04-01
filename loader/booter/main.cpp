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
#include "initrd.h"
#include "processor.h"
#include "storage.h"
#include "filesystem.h"

using Screen::bout;
using Screen::nl;

extern "C" void booter_main(InitRD * pInitrd, MemoryMapEntry * pMemoryMap, uint32 iMemoryMapSize,
                            void * pPlacementAddress, uint32 iBootdrive, uint64 iStartingSector,
                            VideoMode * pVideoMode, void * pFont)
{
    Memory::Initialize(pPlacementAddress);
    Screen::Initialize(pVideoMode, pFont);

    *bout << "Booter: ReaverOS' bootloader 0.2" << nl;
    *bout << "Reading memory map..." << nl << nl;

    Memory::PrintMemoryMap(pMemoryMap, iMemoryMapSize);
    
    *bout << nl << "Reading InitRD..." << nl;
    
//    InitRDDriver::Parse(pInitrd);
    
    *bout << "Entering long mode...";

    Processor::EnterLongMode();

    *bout << " done." << nl;

    // here, we are still in 32bit mode (compatibility mode)
    // however, storage driver's and filesystem driver's code
    // is already 64 bit, so it has no problem with loading
    // data into 64bit areas of memory

    // the same goes for Execute function, which calls 64 bit
    // assembly to do far jump to -2GB in 64bit code sector
    
    Processor::SetupGDT();

    for (;;) ;

/*    *bout << "Loading storage and filesystem drivers...";
    
    StorageDriver storage = InitRDDriver::GetFile("/boot/storage.drv");
    FilesystemDriver fs = InitRDDriver::GetFile("/boot/fs.drv");

    *bout << " done." << nl;
    
    storage.Initialize(iBootdrive);
    fs.Initialize(storage, iStartingSector);

    Processor::SetupNullIDT();
    Processor::EnableInterrupts();
    
    void * end = fs.LoadFileIntoMemoryAddress("/boot/kernel", 0xFFFFFFFF80000000); // -2 GB
    void * placement = fs.LoadFileIntoMemoryAddress("/boot/initrd", Memory::AlignToNextPage(end));

    Processor::DisableInterrupts();
    
    Processor::Execute(0xFFFFFFFF80000000, Memory::AlignToNextPage(end), Memory::MemoryMap(pMemoryMap, iMemoryMapSize), 
                       Memory::AlignToNextPage(placement), iBootdrive, iStartingSector, 
                       Screen::GetProcessedVideoModeDescription());
    
    for (;;);
    
    return;*/
}
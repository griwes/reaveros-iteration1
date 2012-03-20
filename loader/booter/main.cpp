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
 * including commercial applications, adn to alter it and redistribute it
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

extern "C" void __attribute__((cdecl)) booter_main(InitRD * pInitrd, MemoryMapEntry * pMemoryMap, uint32 iMemoryMapSize, 
                                                   void * pPlacementAddress, uint32 iBootdrive, uint64 iStartingSector,
                                                   VideoMode * pVideoMode)
{
    Memory::Initialize(pPlacementAddress);
    Screen::Initialize(pVideoMode);
    
    using Screen::bout;
    using Screen::nl;
    
    bout << "Booter: ReaverOS' bootloader 0.1" << nl;
    bout << "Reading memory map..." << nl;
    
    Memory::PrintMemoryMap(pMemoryMap, iMemoryMapSize);
    
    bout << "Reading InitRD..." << nl;
    
    InitRDDriver::Parse(pInitrd);
    
    bout << "Entering long mode..." << nl;
    
    Processor::EnterLongMode();
    
    StorageDriver * storage = InitRDDriver->GetFile("/boot/storage.drv");
    FilesystemDriver * fs = InitRDDriver->GetFile("/boot/fs.drv");
    
    storage->Initialize(iBootdrive);
    fs->Initialize(storage, iStartingSector);
    
    Processor::SetupNullIDT();
    Processor::EnableInterrupts();
    
    void * end = fs->LoadFileIntoMemoryAddress("/boot/kernel", 0xFFFFFFFF80000000); // -2 GB
    void * placement = fs->LoadFileIntoMemoryAddress("/boot/initrd", Memory::AlignToNextPage(end)); 
    
    Processor::Execute(0xFFFFFFFF80000000, Memory::AlignToNextPage(end), Memory::MemoryMap(pMemoryMap, iMemoryMapSize), 
                       Memory::AlignToNextPage(placement), iBootdrive, iStartingSector, 
                       Screen::GetProcessedVideoModeDescription());
    
    for (;;);
    
    return;
}
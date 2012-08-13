/**
 * ReaverOS
 * kernel/main.cpp
 * Kernel C++-entry point.
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

/**
 * Useful convention is used here, as error in code in kernel in most cases 
 * means critical error: never return error codes - print a message and stop
 * system in called function.
 */

#include "types.h"
#include "memory/memory.h"
#include "processor/processor.h"
#include "screen/screen.h"
#include "memory/memorymap.h"
#include "memory/vmm.h"
#include "screen/console.h"

namespace InitRD
{
    class InitRD;
}

extern "C" void kernel_main(InitRD::InitRD * /*pInitRD*/, Memory::MemoryMapEntry * pMemoryMap, uint32 iMemoryMapSize,
                            void * pPlacementAddress, Screen::VideoMode * pVideoMode, uint8 * pFont)
{
    Memory::PreInitialize(pPlacementAddress);
    Memory::Initialize(pMemoryMap, iMemoryMapSize);

    Screen::Initialize(pVideoMode, pFont);
    
    using Screen::kout;
    using Screen::nl;
    
    *kout << Screen::Gray;

    *kout << "ReaverOS 0.1: Cotyledon" << nl;
    *kout << "Copyright (C) 2011-2012 Reaver Project Team" << nl << nl;
    
    *kout << Screen::White << "[MEM]" << Screen::Gray << " Initializing virtual memory manager... ";

    Memory::InitializeRegions();
    Memory::VMM::MapPage(Memory::VM::VideoBackbufferBase);
    
    *kout << Screen::Green << "done" << Screen::Gray << "." << nl;
    
    *kout << Screen::White << "[VID]" << Screen::Gray << " Initializing boot terminal backbuffer... ";
    
//    Screen::InitializeBackbuffer();
    
    *kout << Screen::Green << "done" << Screen::Gray << "." << nl;

    *kout << Screen::White << "[MEM]" << Screen::Gray << " Printing memory map..." << nl;

    Memory::SystemMemoryMap->PrintMemoryMap();

    *kout << Screen::White << "[VID]" << Screen::Gray << " Printing screen status..." << nl;

    Screen::PrintStatus();

    *kout  << Screen::White << "[CPU]" << Screen::Gray << " Initializing processor... ";
    
    Processor::Initialize();

    *kout << Screen::Green << "done" << Screen::Gray << "." << nl;
    *kout << Screen::White << "[CPU]" << Screen::Gray << " Printing environment info and status..." << nl;

    Processor::PrintStatus();

//    for (;;) ;

    *kout << Screen::White << "[VFS]" << Screen::Gray << " Validating InitRD... ";

//    InitRD::Driver * pInitRDDriver = new InitRD::Driver(pInitRD);
//    pInitRDDriver->Validate();

    *kout << Screen::Green << "done" << Screen::Gray << "." << nl;
    *kout << Screen::White << "[VFS]" << Screen::Gray << " Initializing VFS... ";

//    VFS::Initialize();

    *kout << Screen::Green << "done" << Screen::Gray << "." << nl;
    *kout << Screen::White << "[VFS]" << Screen::Gray << " Mounting InitRD... ";

//    VFS::Mount("/boot", pInitRDDriver);

    *kout << Screen::Green << "done" << Screen::Gray << "." << nl;    
    *kout << Screen::White << "[DRV]" << Screen::Gray << " Initializing driver storage... ";

//    DriverStorage::Initialize();

    *kout << Screen::Green << "done" << Screen::Gray << "." << nl;
    *kout << Screen::White << "[DRV]" << Screen::Gray << " Registering and initializing storage driver... ";

//    DriverStorage::Register(new Drivers::Storage(VFS::LoadFile("/boot/storage.drv")));
//    Drivers::Storage * pStorage = DriverStorage::GetDriver("storage/boot");

    *kout << Screen::Green << "done." << Screen::Gray << "." << nl;
    *kout << Screen::White << "[DRV]" << Screen::Gray << " Printing storage and storage driver details..." << nl;

//    pStorage->PrintDetails();

    *kout << Screen::White << "[DRV]" << Screen::Gray << " Registering and initializing filesystem driver... ";

//    DriverStorage::Register(new Drivers::Filesystem(VFS::LoadFile("/boot/filesystem.drv")));
//    Drivers::Filesystem * pFilesystem = DriverStorage::GetDriver("filesystem/boot", pStorage);

    *kout << Screen::Green << "done." << Screen::Gray << "." << nl;
    *kout << Screen::White << "[DRV]" << Screen::Gray << " Printing filesystem driver details..." << nl;

//    pFilesystem->PrintDetails();

    *kout << Screen::White << "[VFS]" << Screen::Gray << " Looking for system partition... ";

//    VFS::FindSystemPartition(pFilesystem);

    *kout << Screen::Green << "done." << Screen::Gray << "." << nl;
    *kout << Screen::White << "[VFS]" << Screen::Gray << " Printing system partition and its storage device details..." << nl;

//    pFilesystem->PrintParitionDetails();
//    pStorage->PrintDeviceDetails();

    *kout << Screen::White << "[VFS]" << Screen::Gray << " Mounting system partition... ";

//    VFS::Mount("/system", pFilesystem);

    *kout << Screen::Green << "done" << Screen::Gray << "." << nl;
    *kout << Screen::White << "[CONF]" << Screen::Gray << " Loading system configuration... ";

//    Config::Config * pConfig = Config::LoadConfiguration("/system/config.ini");

    *kout << Screen::Green << "done" << Screen::Gray << "." << nl;

    for (;;) ;
    
    return; // for the sake of good coding style
}

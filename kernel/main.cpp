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

/**
 * Useful convention is used here, as error in code in kernel in most cases 
 * mean critical error: never return error codes - print a message and stop
 * system in called function.
 */

#include "types.h"
#include "memory/memory.h"
#include "processor/processor.h"
#include "screen/screen.h"

extern "C" void __attribute__((cdecl)) kernel_main(InitRD::InitRD * pInitRD, Memory::MemoryMap * pMemoryMap,
                                                   void * pPlacementAddress, Screen::VideoMode * pVideoMode)
{
    Memory::PreInitialize(pPlacementAddress);
    Memory::Initialize(pMemoryMap);
    Processor::Initialize();
    
    Screen::Initialize(pVideoMode);
    
    using Screen::kout;
    using Screen::nl;
    
    kout << "ReaverOS 0.1: Cotyledon" << nl << nl;
    
    Processor::PrintStatus();
    
    kout << "Memory is initialized, entire RAM size: " << Memory::GetEntireRAMSize() << ", available RAM size: " 
         << Memory::GetAvailableRAMSize() << nl;
    kout << "Current video mode number: " << Screen::GetVideoModeID() << ", video mode details: "
         << Screen::GetModeDetails().String() << nl;
         
    kout << " - Validating InitRD...";
    
    pInitRD->Validate();
    
    kout << "Done." << nl << " - Loading storage and filesystem drivers...";
    
    DriverStorage::Initialize();
    VFS::Initialize();
    VFS::Mount(InitRD::Filesystem(pInitRD), "/boot");

    /**
     * TODO: detection of root filesystem... at now, I just want to write bootloader code
     * to load kernel and initrd using BIOS interrupts, will write pseudocode here later.
     */
    
    // the following line does much of the remaining job; only process manager is to be initialized
    String shellfilename = Config::ParseConfigFile("/system/kernel.conf")->GetKey("shell.filename");
    
    Process::Manager::Initialize();
    Process::Create(shellfilename).SetAsActive();
    Process::Scheduler::StartUp();
    
    for (;;) ;
    
    return; // for the sake of good coding style
}
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
#include "screen/console.h"

namespace InitRD
{
    class InitRD;
}

extern "C" void kernel_main(InitRD::InitRD * pInitRD, Memory::MemoryMapEntry * pMemoryMap, uint32 iMemoryMapSize,
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

    *kout << "Initializing virtual memory manager... ";

    Memory::InitializeRegions();
    
    *kout << Screen::Green << "done" << Screen::Gray << "." << nl;

    *kout << "Printing memory map..." << nl << nl;

    Memory::SystemMemoryMap->PrintMemoryMap();

    *kout << nl << "Initializing processor... ";
    Processor::Initialize();
    *kout << Screen::Green << "done" << Screen::Gray << "." << nl;
    Processor::PrintStatus();

    for (;;) ;
    
    return; // for the sake of good coding style
}
/**
 * ReaverOS
 * kernel/memory/memory.cpp
 * Main memory subsystem implementation file.
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

#include "memory.h"

void Memory::PreInitialize(void * pPlacementAddress)
{
    Memory::pPlacementAddress = pPlacementAddress;
    
    return;
}

void Memory::Initialize(Memory::MemoryMap * pMemMap)
{
    // recreate paging structures in kernel space
    
    // system status ATM:
    // 1. first 32 MB of physical memory - identity mapped
    // 2. next 32 MB of physical memory - mapped at -2 GB
    // First 32 MBs are special, don't touch them
    // the bootloader is free to do anything with them
    // and we don't want to rewrite this part just because
    // bootloader did something new and important.
    // Kernel and it's InitRD are at -2GB
    
    
}

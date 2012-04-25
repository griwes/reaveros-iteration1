/**
 * ReaverOS
 * loader/booter/processor.h
 * Processor routines.
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

#ifndef __rose_loader_booter_processor_h__
#define __rose_loader_booter_processor_h__

#include "types.h"
#include "memory.h"
#include "screen.h"
#include "paging.h"

namespace Processor
{
    void EnterLongMode();
    void SetupGDT();
    void Execute(uint32, uint64, uint64, uint32, uint64, uint64);

    extern PML4 * PagingStructures;
}

#endif
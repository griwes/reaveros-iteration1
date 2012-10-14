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

#include <cstdint>
#include <cstddef>

#include "screen/screen.h"
#include "memory/memory.h"
#include "memory/manager.h"

void * operator new(uint32_t size)
{
    return memory::default_allocator->allocate(size);
}

void operator delete(void * ptr)
{
    return memory::default_allocator->deallocate(ptr);
}

void * operator new(uint32_t, void * addr)
{
    return addr;
}

extern "C" void __cxa_pure_virtual()
{
    asm ("hlt");
}

void _panic(const char * X, const char * FILE, uint64_t /*LINE*/, const char * FUNC)
{
    if (screen::output)
    {
        screen::line();
        screen::printl("PANIC: \"", X, "\"");
        screen::printl("File: ", FILE); //, ", line ", LINE);
        screen::printl("Function: ", FUNC);
    }
    
    asm ("cli; hlt");
}

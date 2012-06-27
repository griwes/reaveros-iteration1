/**
 * ReaverOS
 * kernel/lib/stack.h
 * Stack header.
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

#ifndef __rose_kernel_lib_stack_h__
#define __rose_kernel_lib_stack_h__

#include "../types.h"
#include "../memory/memorymap.h"

namespace Lib
{
    class Stack
    {
    public:
        Stack(Memory::MemoryMap *, uint64);
        Stack(uint64, uint64, uint64);
        ~Stack();
        
        uint64 Pop();
        void Push(uint64);

        uint64 Count();
    private:
        uint64 * m_pStack;
        uint64 m_iLastPage;
        uint64 m_iSize;
    };
}

#endif
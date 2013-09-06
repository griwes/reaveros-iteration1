/**
 * Reaver Project OS, Rose License
 *
 * Copyright (C) 2011-2013 Reaver Project Team:
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

#include <screen/screen.h>
#include <memory/memory.h>
#include <processor/processor.h>

void * __dso_handle = 0;

void operator delete(void *)
{
}

extern "C" int __cxa_atexit(void (*)(void *), void *, void *)
{
    return 0;
}

extern "C" void __cxa_pure_virtual()
{
    PANIC("Pure virtual function called!");
}

void _panic(const char * message, const char * file, uint64_t line, const char * func, bool additional)
{
//    processor::broadcast(processor::broadcast_types::others, processor::ipis::panic);

#ifdef ROSEDEBUG
    screen::print("\n");
#else
    screen::clear();
#endif

    screen::print(color::red, "Kernel panic: ", color::gray, message, "\n", file, ":", line, ": ", func);

    if (additional)
    {
        screen::print("\nMore information:\n");
    }

    else
    {
        while (true)
        {
            asm volatile ("cli; hlt");
        }
    }
}

extern "C" void * memcpy(void * dest, void * src, uint64_t count)
{
    memory::copy((uint8_t *)src, (uint8_t *)dest, count);

    return dest;
}

extern "C" void * memset(void * dest, int ch, uint64_t count)
{
    memory::set((uint8_t *)dest, (uint8_t)ch, count);

    return dest;
}

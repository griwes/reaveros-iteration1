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
#include <processor/interrupts.h>

void * __dso_handle = 0;

void operator delete(void *)
{
}

extern "C" int __cxa_atexit(void (*)(void *), void *, void *)
{
    return 0;
}

void _panic(const char * message, const char * file, uint64_t line, const char * func)
{
    screen::print(color::red, "\n\nPanic called: ", color::gray, message);
    screen::print("\nFunction: ", func);
    screen::print("\nFile: ", file);
    screen::print("\nLine: ", line);
    
    asm volatile ("cli; hlt");
}

/*void _death(const char * message, const char * file, uint64_t line, const char * func)
{
    screen::clear();
    screen::commit();
    
    screen::print("Kernel panic: ", message);
    screen::print("\n", file, ":", line, ": ", func);
    
    asm volatile ("cli; hlt");
    
    // debugger::start();
}*/

template<>
void _dump_registers(const processor::idt::irq_context &)
{
    
}

template<>
void _dump_registers(const processor::idt::irq_context_error &)
{
    
}

extern "C" void * memcpy(void * dest, void * src, uint64_t count)
{
    memory::copy((uint8_t *)src, (uint8_t *)dest, count);
    
    return dest;
}

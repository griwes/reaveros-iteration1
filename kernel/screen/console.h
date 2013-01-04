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

#pragma once

namespace memory
{
    struct map_entry;
}

namespace screen
{
    struct mode;
    
    extern class kernel_console
    {
    public:
        kernel_console();
        kernel_console(screen::mode *, memory::map_entry *, uint64_t);
        
        void print(char);
        void print(const char *);

        void print(int8_t);
        void print(int16_t);
        void print(int32_t);
        void print(int64_t);
        void print(uint8_t);
        void print(uint16_t);
        void print(uint32_t);
        void print(uint64_t);
        
        template<typename T>
        void print(const T *);
        
        void clear();
        
        void scroll_up();
        void scroll_down();
    } console;
}
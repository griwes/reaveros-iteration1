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

#pragma once

#include <screen/console.h>

namespace memory
{
    struct map_entry;
}

namespace tag
{
    enum tags
    {
        memory,
        cpu,
        acpi,
        scheduler
    };
}

namespace screen
{
    struct mode;

    void initialize(mode *, memory::map_entry *, uint64_t);

    void transaction();
    void commit();
    void clear();

    inline void print()
    {
    }

    inline void print(char c)
    {
        console.lock();
        console.print(c);
        console.unlock();
    }

    inline void print(const char * str)
    {
        console.lock();
        console.print(str);
        console.unlock();
    }

    void print(tag::tags);

    inline void print(color::colors c)
    {
        console.lock();
        console.set_color(c);
        console.unlock();
    }

    template<typename T>
    void print(const T & t)
    {
        console.lock();
        console.print(t);
        console.unlock();
    }

    template<typename T>
    void print(const T * ptr)
    {
        console.lock();
        console.print((void *)ptr);
        console.unlock();
    }

    template<typename First, typename... Rest>
    void print(const First & first, const Rest &... rest)
    {
        console.lock();
        print(first);
        print(rest...);
        console.unlock();
    }

    inline void done()
    {
        console.lock();
        console.special();
        screen::print(color::green, " done.", color::gray);
        console.special(false);
        console.commit();
        screen::print("\n");
        console.unlock();
    }

    template<typename... Args>
    inline void debug(const Args &... args)
    {
#ifdef ROSEDEBUG
        print(args...);
#endif
    }
}

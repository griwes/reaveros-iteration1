/**
 * Reaver Project OS, Rose License
 *
 * Copyright © 2011-2014 Michał "Griwes" Dominiak
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

    void initialize_console();
    void initialize_terminal(mode *, memory::map_entry *, uint64_t);

    inline void clear()
    {
        auto _ = console->lock();
        console->clear();
    }

    inline void print()
    {
    }

    inline void print(char c)
    {
        auto _ = console->lock();
        console->print(c);
    }

    inline void print(const char * str)
    {
        auto _ = console->lock();
        console->print(str);
    }

    void print(tag::tags);

    inline void print(color::colors c)
    {
        auto _ = console->lock();
        console->set_color(c);
    }

    template<typename T>
    void print(const T & t)
    {
        auto _ = console->lock();
        console->print(t);
    }

    template<typename T>
    void print(const T * ptr)
    {
        auto _ = console->lock();
        console->print(static_cast<void *>(ptr));
    }

    template<typename First, typename... Rest>
    void print(const First & first, const Rest &... rest)
    {
        auto _ = console->lock();
        print(first);
        print(rest...);
    }

    inline void done()
    {
        auto _ = console->lock();
        screen::print(color::green, " done.", color::gray);
        screen::print("\n");
    }

    template<typename... Args>
    inline void debug(const Args &... args)
    {
#ifdef ROSEDEBUG
        print(args...);
#endif
    }
}

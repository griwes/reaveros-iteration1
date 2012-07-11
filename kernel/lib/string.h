/**
 * ReaverOS
 * kernel/lib/string.h
 * String header.
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

#ifndef __rose_kernel_lib_string_h__
#define __rose_kernel_lib_string_h__

#include "../types.h"

// TODO: add UTF-8 support

namespace Lib
{
    inline uint64 Strlen(const char * s)
    {
        uint64 len = 0;

        while (*s != 0)
        {
            len++;
            s++;
        }

        return len;
    }

    class String
    {
    public:
        String();
        String(char);
        String(const char *);
        String(const String &);
        ~String();

        uint64 Length() const;
        const char * Buffer() const;
        
        String operator+(const String &);
        String & operator+=(const String &);
        friend String operator+(const char *, const String &);
        String & operator=(const String &);

    private:
        char * m_pData;
        uint64 m_iLength;
    };
}

#endif
/**
 * Reaver Project OS, Rose License
 *
 * Copyright © 2014 Michał "Griwes" Dominiak
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

#include <screen/screen.h>

#include "raf.h"

namespace
{
    bool _compare(const char * null_terminated, const char * sized, uint64_t size)
    {
        for (uint64_t i = 0; i < size; ++i)
        {
            if (!*null_terminated)
            {
                return false;
            }

            if (*null_terminated++ != *sized++)
            {
                return false;
            }
        }

        return true;
    }
}

utils::raf::raf(uint64_t begin, uint64_t end) : _header{ reinterpret_cast<_raf_header *>(begin) }
{
    if (!_header->validate() || end - begin - _header->size > 512)
    {
        PANIC("Provided initial ram disk is not a valid RAF archive.");
    }

    if (_header->version > 1)
    {
        PANIC("Initial ram disk uses an unsupported version of RAF (versions up to 1 are supported).");
    }
}

utils::file utils::raf::operator[](const char * filename) const
{
    uint64_t begin = reinterpret_cast<uint64_t>(_header);
    uint64_t end = begin + _header->size;

    const _file_header * first = reinterpret_cast<const _file_header *>(begin + sizeof(_raf_header));

    for (uint64_t i = 0; i < _header->file_count; ++i)
    {
        if (_compare(filename, first->filename, first->filename_length))
        {
            const uint8_t * start = reinterpret_cast<uint8_t *>(begin + first->offset);
            return { start, start + first->size, filename };
        }
    }

    PANICEX("Couldn't find a file in initial ram disk.", [&]()
    {
        screen::print("Filename: ", filename);
    });
}

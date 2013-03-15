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

namespace processor
{
    class interrupt_entry
    {
    public:
        interrupt_entry() : _valid{}
        {
        }

        void set(uint8_t source, uint32_t global, uint16_t flags)
        {
            _source = source;
            _global_vector = global;
            _flags = flags;
            _valid = true;
        }

        operator bool()
        {
            return _valid;
        }

        uint32_t vector()
        {
            return _global_vector;
        }

        bool standard_polarity()
        {
            return !(_flags & 3);
        }

        bool low()
        {
            return (_flags & 3) == 3;
        }

        bool standard_trigger()
        {
            return !((_flags >> 2) & 3);
        }

        bool level()
        {
            return ((_flags >> 2) & 3) == 3;
        }

    private:
        uint8_t _source;
        uint32_t _global_vector;

        uint16_t _flags;

        bool _valid;
    };
}

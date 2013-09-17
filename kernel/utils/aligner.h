/**
 * Reaver Project OS, Rose License
 *
 * Copyright (C) 2013 Reaver Project Team:
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

namespace utils
{
    namespace _detail
    {
        constexpr uint64_t _is_power_of_two(uint64_t number)
        {
            return (number & (number - 1)) == 0;
        }

        constexpr uint64_t _empty_leading_bits(uint64_t number, uint64_t count = 0)
        {
            return !number ? 64 - count : _empty_leading_bits(number >> 1, count + 1);
        }

        constexpr uint64_t _next_power(uint64_t number)
        {
            return 1ull << (64 - _empty_leading_bits(number));
        }

        constexpr uint64_t _up_to_power(uint64_t number)
        {
            return _is_power_of_two(number) ? number : _next_power(number);
        }
    }

    template<typename T>
    struct aligner
    {
        constexpr static uint64_t size = _detail::_up_to_power(sizeof(T));

        class alignas(size) type : public T
        {
        };
    };
}
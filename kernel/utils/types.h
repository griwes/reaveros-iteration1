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

#pragma once

#include <type_traits>

namespace utils
{
    template<typename T, typename Tag>
    class strong_integral_typedef
    {
    public:
        static_assert(std::is_integral<T>::value, "strong_integral_typedef only works on integrals");

        explicit strong_integral_typedef(T t = {}) : _value{ t }
        {
        }

        template<typename Other>
        auto & operator += (const Other & other)
        {
            _value += static_cast<T>(other);
            return *this;
        }

        template<typename Other>
        auto & operator -= (const Other & other)
        {
            _value -= static_cast<T>(other);
            return *this;
        }

        explicit operator T() const
        {
            return _value;
        }

    private:
        T _value;
    };
}

using phys_addr_t = utils::strong_integral_typedef<uint64_t, class physical_address_tag>;
using virt_addr_t = utils::strong_integral_typedef<uint64_t, class virtual_address_tag>;
using pci_vendor_t = utils::strong_integral_typedef<uint16_t, class pci_vendor_tag>;

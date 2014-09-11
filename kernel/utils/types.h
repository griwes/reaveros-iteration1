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

class virtual_address_tag;

namespace utils
{
    template<typename T, typename Tag>
    class strong_integral_typedef
    {
    public:
        static_assert(std::is_integral<T>::value, "strong_integral_typedef only works on integrals");
        template<typename, typename>
        friend class strong_integral_typedef;

        strong_integral_typedef() = default;

        explicit strong_integral_typedef(T t) : _value{ t }
        {
        }

        template<typename U, typename std::enable_if<sizeof(T) >= sizeof(U), int>::type = 0>
        strong_integral_typedef(const strong_integral_typedef<U, Tag> & other) : _value{ other._value }
        {
        }

        template<typename U, typename std::enable_if<std::is_same<T, uint64_t>::value && std::is_same<::virtual_address_tag, Tag>::value, decltype(std::declval<U *>(), int{})>::type = 0>
        strong_integral_typedef(U * ptr) : _value{ reinterpret_cast<uint64_t>(ptr) }
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

        template<typename Other>
        auto operator + (const Other & other) const
        {
            return strong_integral_typedef{ _value + static_cast<T>(other) };
        }

        template<typename Other>
        auto operator - (const Other & other) const
        {
            return strong_integral_typedef{ _value - static_cast<T>(other) };
        }

#define DEFINE_OPERATOR(op)                          \
        template<typename Other>                     \
        auto operator op (const Other & other) const \
        {                                            \
            return _value op static_cast<T>(other);  \
        }

        DEFINE_OPERATOR(==)
        DEFINE_OPERATOR(!=)
        DEFINE_OPERATOR(<)
        DEFINE_OPERATOR(<=)
        DEFINE_OPERATOR(>)
        DEFINE_OPERATOR(>=)

#undef DEFINE_OPERATOR

#define DEFINE_OPERATOR(op)                          \
        template<typename Other>                     \
        auto operator op (const Other & other) const \
        {                                            \
            return _value op other;                  \
        }

        DEFINE_OPERATOR(>>)
        DEFINE_OPERATOR(<<)

#undef DEFINE_OPERATOR

#define DEFINE_OPERATOR(op)                              \
        template<typename Other>                         \
        decltype(auto) operator op (const Other & other) \
        {                                                \
            _value op static_cast<T>(other);             \
            return *this;                                \
        }

        DEFINE_OPERATOR(&=)
        DEFINE_OPERATOR(|=)
        DEFINE_OPERATOR(^=)

#undef DEFINE_OPERATOR

        explicit operator T() const
        {
            return _value;
        }

        explicit operator bool() const
        {
            return static_cast<bool>(_value);
        }

        template<typename U, typename std::enable_if<std::is_same<::virtual_address_tag, Tag>::value, decltype(std::declval<U *>(), int{})>::type = 0>
        operator U *()
        {
            return reinterpret_cast<U *>(_value);
        }

    private:
        T _value = 0;
    };
}

using phys_addr_t = utils::strong_integral_typedef<uint64_t, class physical_address_tag>;
using phys_addr32_t = utils::strong_integral_typedef<uint32_t, class physical_address_tag>;
using virt_addr_t = utils::strong_integral_typedef<uint64_t, class virtual_address_tag>;
using pci_vendor_t = utils::strong_integral_typedef<uint16_t, class pci_vendor_tag>;

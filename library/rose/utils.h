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

#include <utility>
#include <cstdint>

namespace rose
{
    inline std::pair<std::uint64_t, std::uint64_t> get_allowed_address_range()
    {
        return std::make_pair(0, 0x800000000000);
    }

    std::uint64_t syscall(std::uint64_t number);
    std::uint64_t syscall(std::uint64_t number, std::uint64_t rsi);
    std::uint64_t syscall(std::uint64_t number, std::uint64_t rsi, std::uint64_t rdi);
    std::uint64_t syscall(std::uint64_t number, std::uint64_t rsi, std::uint64_t rdi, std::uint64_t r8);
    std::uint64_t syscall(std::uint64_t number, std::uint64_t rsi, std::uint64_t rdi, std::uint64_t r8, std::uint64_t r9);
    std::uint64_t syscall(std::uint64_t number, std::uint64_t rsi, std::uint64_t rdi, std::uint64_t r8, std::uint64_t r9, std::uint64_t r10);
    std::uint64_t syscall(std::uint64_t number, std::uint64_t rsi, std::uint64_t rdi, std::uint64_t r8, std::uint64_t r9, std::uint64_t r10, std::uint64_t r12);
    std::uint64_t syscall(std::uint64_t number, std::uint64_t rsi, std::uint64_t rdi, std::uint64_t r8, std::uint64_t r9, std::uint64_t r10, std::uint64_t r12,
        std::uint64_t r13);
    std::uint64_t syscall(std::uint64_t number, std::uint64_t rsi, std::uint64_t rdi, std::uint64_t r8, std::uint64_t r9, std::uint64_t r10, std::uint64_t r12,
        std::uint64_t r13, std::uint64_t r14);
    std::uint64_t syscall(std::uint64_t number, std::uint64_t rsi, std::uint64_t rdi, std::uint64_t r8, std::uint64_t r9, std::uint64_t r10, std::uint64_t r12,
        std::uint64_t r13, std::uint64_t r14, std::uint64_t r15);
}

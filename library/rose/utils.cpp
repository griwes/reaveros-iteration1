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

#include "utils.h"

std::uint64_t rose::syscall(std::uint64_t number)
{
    asm volatile (R"(
        syscall
    )"
    :
        "=r"(number)
    :
        "a"(number)
    :
        "memory"
    );

    return number;
}

std::uint64_t rose::syscall(std::uint64_t number, std::uint64_t rsi)
{
    asm volatile (R"(
        syscall
    )"
    :
        "=r"(number)
    :
        "a"(number),
        "S"(rsi)
    :
        "memory"//, "%rax", "%rsi"
    );

    return number;
}

std::uint64_t rose::syscall(std::uint64_t number, std::uint64_t rsi, std::uint64_t rdi)
{
    asm volatile (R"(
        syscall
    )"
    :
        "=r"(number)
    :
        "a"(number),
        "S"(rsi),
        "D"(rdi)
    :
        "memory"//, "%rax", "%rsi", "%rdi"
      );

    return number;
}

std::uint64_t rose::syscall(std::uint64_t number, std::uint64_t rsi, std::uint64_t rdi, std::uint64_t r8)
{
    asm volatile (R"(
        mov %[r8], %%r8

        syscall
    )"
    :
        "=r"(number)
    :
        "a"(number),
        "S"(rsi),
        "D"(rdi),
        [r8]"r"(r8)
    :
        "memory"//, "%rax", "%rsi", "%rdi", "%r8"
    );

    return number;
}

std::uint64_t rose::syscall(std::uint64_t number, std::uint64_t rsi, std::uint64_t rdi, std::uint64_t r8, std::uint64_t r9)
{
    asm volatile (R"(
        mov %[r8], %%r8
        mov %[r9], %%r9

        syscall
    )"
    :
        "=r"(number)
    :
        "a"(number),
        "S"(rsi),
        "D"(rdi),
        [r8]"r"(r8),
        [r9]"r"(r9)
    :
        "memory"//, "%rax", "%rsi", "%rdi", "%r8", "%r9"
    );

    return number;
}

std::uint64_t rose::syscall(std::uint64_t number, std::uint64_t rsi, std::uint64_t rdi, std::uint64_t r8, std::uint64_t r9, std::uint64_t r10)
{
    asm volatile (R"(
        mov %[r8], %%r8
        mov %[r9], %%r9
        mov %[r10], %%r10

        syscall
    )"
    :
        "=r"(number)
    :
        "a"(number),
        "S"(rsi),
        "D"(rdi),
        [r8]"r"(r8),
        [r9]"r"(r9),
        [r10]"r"(r10)
    :
        "memory"//, "%rax", "%rsi", "%rdi", "%r8", "%r9", "%r10"
    );

    return number;
}

std::uint64_t rose::syscall(std::uint64_t number, std::uint64_t rsi, std::uint64_t rdi, std::uint64_t r8, std::uint64_t r9, std::uint64_t r10, std::uint64_t r12)
{
    asm volatile (R"(
        mov %[r8], %%r8
        mov %[r9], %%r9
        mov %[r10], %%r10
        mov %[r12], %%r12

        syscall
    )"
    :
        "=r"(number)
    :
        "a"(number),
        "S"(rsi),
        "D"(rdi),
        [r8]"r"(r8),
        [r9]"r"(r9),
        [r10]"r"(r10),
        [r12]"r"(r12)
    :
        "memory"//, "%rax", "%rsi", "%rdi", "%r8", "%r9", "%r10", "%r12"
    );

    return number;
}

std::uint64_t rose::syscall(std::uint64_t number, std::uint64_t rsi, std::uint64_t rdi, std::uint64_t r8, std::uint64_t r9, std::uint64_t r10, std::uint64_t r12, std::uint64_t r13)
{
    asm volatile (R"(
        mov %[r8], %%r8
        mov %[r9], %%r9
        mov %[r10], %%r10
        mov %[r12], %%r12
        mov %[r13], %%r13

        syscall
    )"
    :
        "=r"(number)
    :
        "a"(number),
        "S"(rsi),
        "D"(rdi),
        [r8]"r"(r8),
        [r9]"r"(r9),
        [r10]"r"(r10),
        [r12]"r"(r12),
        [r13]"r"(r13)
    :
        "memory"//, "%rax", "%rsi", "%rdi", "%r8", "%r9", "%r10", "%r12", "%r13"
    );

    return number;
}

std::uint64_t rose::syscall(std::uint64_t number, std::uint64_t rsi, std::uint64_t rdi, std::uint64_t r8, std::uint64_t r9, std::uint64_t r10, std::uint64_t r12, std::uint64_t r13,
    std::uint64_t r14)
{
    asm volatile (R"(
        mov %[r8], %%r8
        mov %[r9], %%r9
        mov %[r10], %%r10
        mov %[r12], %%r12
        mov %[r13], %%r13
        mov %[r14], %%r14

        syscall
    )"
    :
        "=r"(number)
    :
        "a"(number),
        "S"(rsi),
        "D"(rdi),
        [r8]"r"(r8),
        [r9]"r"(r9),
        [r10]"r"(r10),
        [r12]"r"(r12),
        [r13]"r"(r13),
        [r14]"m"(r14)
    :
        "memory"//, "%rax", "%rsi", "%rdi", "%r8", "%r9", "%r10", "%r12", "%r13", "%r14"
    );

    return number;
}

std::uint64_t rose::syscall(std::uint64_t number, std::uint64_t rsi, std::uint64_t rdi, std::uint64_t r8, std::uint64_t r9, std::uint64_t r10, std::uint64_t r12, std::uint64_t r13,
    std::uint64_t r14, std::uint64_t r15)
{
    asm volatile (R"(
        mov %[r8], %%r8
        mov %[r9], %%r9
        mov %[r10], %%r10
        mov %[r12], %%r12
        mov %[r13], %%r13
        mov %[r14], %%r14
        mov %[r15], %%r15

        syscall
    )"
    :
        "=r"(number)
    :
        "a"(number),
        "S"(rsi),
        "D"(rdi),
        [r8]"r"(r8),
        [r9]"r"(r9),
        [r10]"r"(r10),
        [r12]"r"(r12),
        [r13]"m"(r13),
        [r14]"m"(r14),
        [r15]"m"(r15)
      :
        "memory"//, "%rax", "%rsi", "%rdi", "%r8", "%r9", "%r10", "%r12", "%r13", "%r14", "%r15"
      );

    return number;
}

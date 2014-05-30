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

namespace processor
{
    struct syscall_context;

    namespace syscalls
    {
        enum class syscalls : uint64_t
        {
            // service syscalls
            service_kernel_console_print = 0,
            service_get_initrd = 1,
            service_spawn = 2,

            // regular syscalls
            exit = 100
        };

        void initialize();

        using handler = void (*)(uint64_t, processor::syscall_context &);
        void register_syscall(syscalls syscall, handler hnd, uint64_t context = 0, bool service_only = false);
    }
}

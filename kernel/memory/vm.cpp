/**
 * Reaver Project OS, Rose License
 *
 * Copyright © 2013 Michał "Griwes" Dominiak
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

#include <atomic>

#include <memory/vm.h>
#include <memory/x64paging.h>
#include <processor/processor.h>
#include <screen/screen.h>

namespace
{
    std::atomic<uint64_t> _lowest;
}

void memory::vm::initialize()
{
    x64::pml4 * boot_vas = processor::get_cr3();
    (*boot_vas)[256] = processor::get_asid();

    processor::reload_cr3();

    new (&_lowest) std::atomic<uint64_t>{ 0xFFFFFFFF80000000 };
}

virt_addr_t memory::vm::allocate_address_range(uint64_t size)
{
    auto real_size = size + 4095;
    real_size &= ~4095ull;

    auto ret = _lowest.fetch_sub(real_size) - real_size;

    if (ret < 0xFFFF800000000000)
    {
        PANIC("virtual memory exhausted!");
    }

    screen::debug("\nRequested ", size, " bytes of address space, allocating ", real_size, " bytes at ", reinterpret_cast<void *>(ret));

    return virt_addr_t{ ret };
}

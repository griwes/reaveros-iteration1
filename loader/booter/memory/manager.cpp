/**
 * Reaver Project OS, Rose License
 *
 * Copyright © 2011-2012 Michał "Griwes" Dominiak
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

#include <memory/manager.h>
#include <memory/memory.h>
#include <memory/x64paging.h>

memory::manager::placement_allocator::placement_allocator(uint32_t placement)
    : placement_address{ placement }, _current{}, _leeched{}, top_mapped{ 64 * 1024 * 1024 - 1 }
{
    _leeched = memory::map::next_usable(0x100000 - 1);
    _current = memory::map::allocate_empty_entry(_leeched);
    _current->type = 5;
    _current->length = placement < 0x100000 ? 0 : placement - 0x100000;
}

memory::manager::placement_allocator::~placement_allocator()
{
}

void memory::manager::placement_allocator::save()
{
    placement_address += 4095;
    placement_address &= ~(uint32_t)4095;

    _current->length = placement_address - _current->base;

    if (_current->type == 3)
    {
        return;
    }

    map_entry * next = memory::map::allocate_empty_entry(_leeched);

    switch (_current->type)
    {
        case 5:
            next->type = 2;
            break;
        case 2:
            next->type = 3;
    }

    _current = next;
}

void * memory::manager::placement_allocator::allocate(uint32_t size)
{
    if (!size)
    {
        return nullptr;
    }

    static bool vas_context = false;

    if (top_mapped - placement_address <= 3 * 4096 && !vas_context)
    {
        vas_context = true;

        vas->map(top_mapped + 1, top_mapped + 1 + 64 * 1024 * 1024, top_mapped + 1);
        top_mapped += 64 * 1024 * 1024;

        vas_context = false;
    }

    size += 15;
    size &= ~(uint32_t)15;

    while (placement_address + size >= _leeched->base + _leeched->length)
    {
        _leeched = memory::map::next_usable(placement_address);

        if (_leeched == nullptr)
        {
            PANIC("Not enough memory installed on the system.");
        }

        placement_address = _leeched->base;

        auto next = memory::map::allocate_empty_entry(_leeched);
        next->type = _current->type;
        _current = next;
    }

    auto ret = placement_address;
    placement_address += size;

    while (placement_address >= _current->base + _current->length)
    {
        _current->length += 4096;
        _leeched->base += 4096;
        _leeched->length -= 4096;
    }

    return (void *)ret;
}

void memory::manager::placement_allocator::deallocate(void *)
{
    // no-op for placement

    return;
}

memory::manager::backwards_allocator::backwards_allocator()
{
}

memory::manager::backwards_allocator::~backwards_allocator()
{
}

void * memory::manager::backwards_allocator::allocate(uint32_t size)
{
    uint32_t top_placement = memory::map::find_last_usable(size);
    return (void *)top_placement;
}

void memory::manager::backwards_allocator::deallocate(void *)
{
    // no-op for backwards

    return;
}

void memory::manager::placement_allocator::align(uint32_t a)
{
    placement_address += a - 1;
    placement_address &= ~(a - 1);
}

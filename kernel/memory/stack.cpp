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

#include <memory/stack.h>
#include <memory/map.h>

memory::pmm::frame_stack::frame_stack(memory::map_entry * map, uint64_t map_size)
{
    for (uint64_t i = 0; i < map_size; ++i)
    {
        if (map[i].type == 1 && (map[i].base >= 1024 * 1024 || map[i].base + map[i].length > 1024 * 1024))
        {
            for (uint64_t frame = (map[i].base < 1024 * 1024) ? (1024 * 1024) : ((map[i].base + 4095) & ~(uint64_t)4095);
                frame < map[i].base + map[i].length; frame += 4096)
            {
                push(frame);
            }
        }
    }
}

uint64_t memory::pmm::frame_stack::pop()
{

}

void memory::pmm::frame_stack::pop_chunk()
{

}

void memory::pmm::frame_stack::push(uint64_t )
{

}

void memory::pmm::frame_stack::push_chunk(memory::pmm::frame_stack_chunk * )
{

}

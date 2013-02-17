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

#include <memory/stacks.h>
#include <memory/index_stack.h>
#include <memory/vm.h>
#include <screen/screen.h>
#include <processor/current_core.h>

namespace
{
    memory::index_stack _global_stack_stack;
}

void memory::stack_manager::initialize()
{
    new ((void *)&_global_stack_stack) memory::index_stack(vm::global_stack_stack_area, 0, 64 * 1024 - 1, 64 * 1024 * 1024);   
    // number of threads limited to 64 * 1024 * 1024 should be sane
    // doesn't work without -1; locks somewhere in mapping it (?!)
}

void memory::stack_manager::split_stack_stack(processor::core * cores, uint64_t num_cores)
{
    uint64_t stacks_to_distribute = _global_stack_stack.size() / 2;
    uint64_t stacks_per_core = stacks_to_distribute / (num_cores + 1);
    
    if (stacks_per_core > (1024 * 1024) / 8)
    {
        stacks_per_core = (1024 * 1024) / 8;
    }
    
    screen::debug("\n", stacks_to_distribute, " stacks to distribute, ", stacks_per_core, " per core");
    
    screen::debug("\nFilling stack stack of CPU#", processor::current_core::id());
    
    for (uint64_t i = 0; i < stacks_per_core; ++i)
    {
        processor::current_core::stack_stack().push(_global_stack_stack.pop());
    }
    
    for (uint64_t i = 0; i < num_cores; ++i)
    {
        screen::debug("\nFilling stack stack of CPU#", cores[i].apic_id());
        
        for (uint64_t j = 0; j < stacks_per_core; ++j)
        {
            cores[i].stack_stack().push(_global_stack_stack.pop());
        }
    }
}

uint64_t memory::stack_manager::allocate()
{
    if (processor::ready())
    {
        return memory::vm::stack_area + (processor::current_core::stack_stack().pop() + 1) * 2 * 4096;
    }
    
    return memory::vm::stack_area + (_global_stack_stack.pop() + 1) * 2 * 4096;
}

void memory::stack_manager::free(uint64_t stack)
{
    stack += 8095;
    stack &= ~(uint64_t)8095;
    
    if (processor::ready())
    {
        processor::current_core::stack_stack().push((stack - memory::vm::stack_area) / (2 * 4096) - 1);
    }
}

memory::index_stack * memory::stack_manager::global_stack_stack()
{
    return &_global_stack_stack;
}

extern "C" void _move_stack(uint64_t);

void memory::stack_manager::set(uint64_t stack)
{
    _move_stack(stack);
}

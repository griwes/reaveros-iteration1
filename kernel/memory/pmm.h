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

#pragma once

namespace processor
{
    class core;
}

namespace memory
{
    struct map_entry;
    
    namespace pmm
    {
        void initialize(map_entry *, uint64_t);
        
        uint64_t pop();
        void push(uint64_t);
        
        class frame_stack
        {
        public:
            frame_stack();
            frame_stack(map_entry *, uint64_t);
            frame_stack(uint64_t);
            
            uint64_t pop();
            void push(uint64_t);
            
            uint64_t size()
            {
                return _size;
            }
            
        private:
            void _expand();
            void _shrink();
            
            uint64_t * _stack;
            uint64_t _size;
            uint64_t _capacity;
            
            uint8_t _lock;
        };
        
        void boot_report();
        
        void split_frame_stack(processor::core *, uint64_t);
    }
}

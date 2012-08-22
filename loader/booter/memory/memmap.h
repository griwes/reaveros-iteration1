#pragma once

#include <cstdint>
#include <cstddef>

#include "../processor/processor.h"

namespace memory
{
    class map_entry_t
    {
    public:
        uint64_t base;
        uint64_t length;
        uint32_t type;
        uint32_t extended_attribs;
    } __attribute__((packed));
    
    class kernel_map_entry_t : public map_entry_t
    {
    public:
        uint32_t proximity_domain;
    } __attribute__((packed));
    
    class chained_map_entry_t : public kernel_map_entry_t
    {
    public:
        chained_map_entry_t * prev;
        chained_map_entry_t * next;
    } __attribute__((packed));
    
    class kernel_map_t
    {
    public:
        kernel_map_entry_t * map;
    };
    
    class map_t
    {
    public:
        map_t(map_entry_t *, uint32_t);
        ~map_t();
        
        map_t * sanitize();
        void apply_numa(processor::numa_env_t *);
        
        kernel_map_t * generate_kernel_map(uint64_t);
        
        bool usable(uint64_t, uint32_t = -1);
        uint64_t next_usable(uint64_t, uint32_t = -1);
        
    private:
        map_entry_t * sequence_entries;
        chained_map_entry_t * entries;
        uint32_t num_entries;
        bool sane;
    };
}

#include <stdint.h>
#include <stddef.h>

namespace memory
{
    class map_entry_t;
    class map_t;
    
    void initialize(uint32_t);
}

namespace screen
{
    class boot_mode_t;
    
    void initialize(boot_mode_t *, void *);
    
    template<typename... T>
    void printl(T &... a, uint32_t = 1);
    
    template<typename... T>
    void print(T &... a);
    
    template<typename... T>
    void printf(const char *, T &... a);
    
    template<typename T>
    void print(T &);
}

extern "C" void __attribute__((cdecl)) booter_main(memory::map_entry_t * memory_map, uint32_t memory_map_size, 
                uint32_t placement, uint32_t kernel, uint32_t kernel_size, uint32_t initrd_size, 
                screen::boot_mode_t video_mode, void * font)
{
    memory::initialize(placement);
    screen::initialize(video_mode, font);
    
    memory::map_t mem_map(memory_map, memory_map_size);
    
    screen::printl("Booter, Reaver Project Bootloader v0.3");
    screen::printl("Copyrights (C) 2012 Reaver Project Team", 2);
    
    screen::printl("[MEM] Reading memory map...");
    
    screen::printl(mem_map, 2);
    
    screen::printl("[MEM] Sanitizing memory map...");
    
    memory::map_t * sane_map = mem_map.sanitize();
}
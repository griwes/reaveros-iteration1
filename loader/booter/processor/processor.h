#ifndef __rose_loader_booter_processor_h__
#define __rose_loader_booter_processor_h__

namespace processor
{
    class numa_env;
    
    void check_long_mode();
    void enter_long_mode();
    void setup_gdt();
}

#endif

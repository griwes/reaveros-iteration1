#include <screen/screen.h>
#include <screen/mode.h>
#include <memory/memory.h>
#include <memory/memmap.h>
#include <processor/processor.h>
#include <scheduler/scheduler.h>
#include <scheduler/process.h>
#include <drivers/initrd.h>

namespace screen
{
    class mode;
    
    namespace tag
    {
        enum
        {
            memory,
            screen,
            cpu,
            scheduler
        };
    }
    
    void done();
    void initialize();
    void initialize_server();
    void print();
}

namespace memory
{
    class map;
    
    namespace pmm
    {
        void initialize();
    }
    
    namespace vmm
    {
        void initialize();
    }
    
    void report();
}

namespace processor
{
    void initialize();
}

namespace scheduler
{
    void initialize();
    void create_process();
    
    class process;
}

namespace tag = screen::tag;

extern "C" void __attribute__((cdecl)) kernel_main(memory::map * mem_map, screen::mode * video, initrd::initrd * initrd)
{
    screen::initialize(video);
    
    screen::print("ReaverOS: Reaver Project Operating System \"Rose\"\n");
    screen::print("Version: 0.1, Codename \"Cotyledon\"\n");
    screen::print("Copyright (C) 2012 Reaver Project Team\n\n");
    
    screen::print(tag::memory, "Initializing physical memory manager...");
    memory::pmm::initialize(mem_map);
    screen::done();
    
    screen::print(tag::memory, "Reporting memory manager status...");
    memory::report();
    
    screen::print(tag::cpu, "Initializing processor...");
    processor::initialize();
    screen::done();
    
    screen::print(tag::scheduler, "Initializing scheduler...");
    scheduler::initialize();
    
    screen::print(tag::scheduler, "Initializing virtual memory manager server...");
    scheduler::process vmm = scheduler::create_process(initrd["vmm.srv"]);
    screen::done();
    
    screen::print(tag::scheduler, "Starting video server...");
    scheduler::process vsrv = scheduler::create_process(initrd["video.srv"]);
    screen::done();
    
    screen::print(tag::screen, "Switching to video server output...");
    screen::initialize_server(vsrv);
    screen::done();
    
    screen::print(tag::scheduler, "Starting device manager server...");
    scheduler::process device_mgr = scheduler::create_process(initrd["devicemgr.srv"]);
    screen::done();
    
    screen::print(tag::scheduler, "Starting VIOLA server...");
    scheduler::process viola = scheduler::create_process(initrd["viola.srv"]);
    screen::done();
    
    screen::print(tag::scheduler, "Starting storage device driver...");
    scheduler::process storage = scheduler::create_process(initrd["storage.srv"]);
    screen::done();
    
    screen::print(tag::scheduler, "Starting filesystem driver...");
    scheduler:: process filesystem = scheduler::create_process(initrd["filesystem.srv"]);
    screen::done();
}

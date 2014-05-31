#include <rose/process.h>
#include <rose/file.h>
#include <rose/utils.h>

namespace rose
{
    namespace service
    {
        namespace init
        {
            class initrd
            {

            };

            initrd get_initrd();
        }

        process spawn(file);
    }

    namespace process_manager
    {
        enum calls
        {
            register_init,
            create_process
        };
    }
}

namespace rose
{
    void __print(const char * string)
    {
        rose::syscall(0, reinterpret_cast<uint64_t>(string));
    }
}

extern "C" void initsrv_main()
{
    rose::__print("\n\n");
    rose::__print("\\{ffffff}[Init     ]\\{bbbbbb} Rose Init Server v0.0.1 dev\n");
    rose::__print("\\{ffffff}[Init     ]\\{bbbbbb} Copyright © 2014 Reaver Project Team\n");
    rose::__print("\\{ffffff}[Init     ]\\{bbbbbb} 1/1: Spawning process manager... ");

    for (;;) ;

/*    auto initrd = rose::service::init::get_initrd();
    auto process_manager = rose::service::spawn(initrd["procmgr.srv"]);

    process_manager.send(rose::process_manager::register_init, rose::get_pid());
*/
//    rose::process vmm = process_manager.send<rose::process>(rose::process_manager::create_process, initrd["vmm.srv"]).get();
}

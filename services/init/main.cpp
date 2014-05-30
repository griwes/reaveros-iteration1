#include <rose/process.h>
#include <rose/file.h>

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
            register_initrd,
            create_process
        };
    }
}

namespace rose
{
    void __print(const char * string)
    {
        asm volatile (R"(
            mov $100, %%rax

            syscall
        )" :: "b"(string) : "memory");
    }
}

extern "C" void initsrv_main()
{
    for (;;) rose::__print("hello from userspace!");

    for (;;) ;

/*    auto initrd = rose::service::init::get_initrd();
    auto process_manager = rose::service::spawn(initrd["procmgr.srv"]);

    process_manager.send(rose::process_manager::register_initrd, rose::get_pid());
*/
//    rose::process vmm = process_manager.send<rose::process>(rose::process_manager::create_process, initrd["vmm.srv"]).get();
}

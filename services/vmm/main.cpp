#include <vector>
#include <string>

#include <rose/rose.h>
#include <rose/service.h>

#include "vmm.h"

namespace rose
{
    namespace vmm
    {
        std::vector<rose::vmm::address_space> address_spaces;
        std::pair<uint64_t, uint64_t> allowed_addresses;
    }
}

int main()
{
    rose::vmm::allowed_addresses = rose::get_allowed_address_range();
    
    rose::register_handler(rose::ipc, rose::pool(16), rose::vmm::receive);
    
    while (rose::get_signal() != rose::signal::term)
    {
    }
    
    return 0;
}

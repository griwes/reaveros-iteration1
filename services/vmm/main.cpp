/**
 * Reaver Project OS, Rose License
 *
 * Copyright © 2011-2013 Michał "Griwes" Dominiak
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
 **/

#include <rose/rose.h>
#include <rose/service.h>

#include "vmm.h"

namespace rose
{
    namespace vmm
    {
        std::map<rose::process, rose::vmm::address_space> address_spaces;
        std::pair<uint64_t, uint64_t> allowed_addresses;
    }
}

int main()
{
    rose::service::per_thread_foreign(true);

    rose::vmm::allowed_addresses = rose::get_allowed_address_range();

    rose::register_handler(rose::signal::ipc, rose::pool{16}, rose::vmm::receive);
    rose::service::register_handler(rose::service::exceptions::pf, rose::pool{16}, rose::vmm::handle_pf);

    while (rose::get_signal() != rose::signal::term)
    {
    }

    return 0;
}

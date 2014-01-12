/**
 * Reaver Project OS, Rose License
 *
 * Copyright © 2013 Michał "Griwes" Dominiak
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

namespace utils
{
    template<typename T>
    class mmio_helper
    {
    public:
        mmio_helper(uint64_t base) : _base{ base }
        {
        }

        void operator()(uint64_t reg, T value)
        {
            *(volatile T *)(_base + reg) = value;
        }

        T operator()(uint64_t reg)
        {
            return *(volatile T *)(_base + reg);
        }

    private:
        uint64_t _base;
    };
}

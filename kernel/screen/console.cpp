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

#include <screen/console.h>
#include <screen/terminal.h>

namespace screen
{
    kernel_console console;
}

namespace
{
    int64_t _lpt_port = 0;
}

screen::kernel_console::kernel_console(terminal * term) : _terminal{ term }
{
    if (_lpt_port == 0)
    {
        _lpt_port = *(uint16_t *)0x408;

        if (!_lpt_port)
        {
            _lpt_port = -1;
        }
    }
}

void screen::kernel_console::clear()
{
    if (_terminal)
    {
        _terminal->clear();
    }
}

void screen::kernel_console::print(char c)
{
    if (_terminal)
    {
        _terminal->put_char(c);
    }

    if (c != '\0' && _lpt_port != -1)
    {
        outb(_lpt_port, (unsigned char)c);
        outb(_lpt_port + 2, 0x0c);
        outb(_lpt_port + 2, 0x0d);
    }
}

void screen::kernel_console::print(const char * str)
{
    while (*str)
    {
        if (*str == '\\' && *(str + 1) == '{')
        {
            auto old = str;
            str += 2;

            // XXRRGGBB
            uint32_t color = 0;
            uint8_t nibble = 0;

            while (nibble != 6 && *str)
            {
                if (*str >= '0' && *str <= '9')
                {
                    color |= (*str - '0') << (nibble * 4);
                }

                else if (*str >= 'a' && *str <= 'f')
                {
                    color |= (*str - 'a' + 10) << (nibble * 4);
                }

                else if (*str >= 'A' && *str <= 'F')
                {
                    color |= (*str - 'A' + 10) << (nibble * 4);
                }

                ++str;
                ++nibble;
            }

            if (nibble != 6 || *str != '}')
            {
                str = old;
            }

            else if (_terminal)
            {
                ++str;
                _terminal->set_color(static_cast<color::colors>(color));
            }
        }

        if (_lpt_port != -1)
        {
            outb(_lpt_port, (unsigned char)*str);
            outb(_lpt_port + 2, 0x0c);
            outb(_lpt_port + 2, 0x0d);
        }

        if (_terminal)
        {
            _terminal->put_char(*str);
        }

        ++str;
    }
}

void screen::kernel_console::set_color(color::colors c)
{
    if (_terminal)
    {
        _terminal->set_color(c);
    }
}

namespace
{
    template<typename T>
    void _print_int(T t)
    {
        if (t == 0)
        {
            screen::console.print('0');

            return;
        }

        if (!(t > 0 || t == 0)) // supress [T = unsigned] unsigned comparison t < 0 warning
        {
            screen::console.print('-');
            t = -t;
        }

        T div = t / 10;
        T mod = t % 10;

        if (div != 0)
        {
            _print_int(div);
        }

        screen::console.print((char)('0' + mod));
    }
}

void screen::kernel_console::print(int8_t i)
{
    _print_int(i);
}

void screen::kernel_console::print(int16_t i)
{
    _print_int(i);
}

void screen::kernel_console::print(int32_t i)
{
    _print_int(i);
}

void screen::kernel_console::print(int64_t i)
{
    _print_int(i);
}

void screen::kernel_console::print(uint8_t i)
{
    _print_int(i);
}

void screen::kernel_console::print(uint16_t i)
{
    _print_int(i);
}

void screen::kernel_console::print(uint32_t i)
{
    _print_int(i);
}

void screen::kernel_console::print(uint64_t i)
{
    _print_int(i);
}

void screen::kernel_console::print(void * ptr)
{
    print("0x");

    for (uint64_t i = 64; i > 0; i -= 4)
    {
        print("0123456789ABCDEF"[(((uint64_t)ptr) >> (i - 4)) & 0xF]);
    }
}

void screen::kernel_console::print(pci_vendor_t vendor)
{
    print("0x");

    for (uint64_t i = 16; i > 0; i -= 4)
    {
        print("0123456789ABCDEF"[(vendor.vendor >> (i - 4)) & 0xF]);
    }
}

utils::unique_lock<utils::recursive_spinlock> screen::kernel_console::lock()
{
    return { _semaphore };
}

void screen::kernel_console::drop_locking()
{
    // terrible, terrible hack
    new (&_semaphore) utils::recursive_spinlock{};
}

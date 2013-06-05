/**
 * Reaver Project OS, Rose License
 *
 * Copyright (C) 2011-2013 Reaver Project Team:
 * 1. Michał "Griwes" Dominiak
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
 * Michał "Griwes" Dominiak
 *
 **/

#include <screen/console.h>
#include <screen/terminal.h>
#include <processor/current_core.h>

namespace screen
{
    kernel_console console;
}

namespace
{
    struct message
    {
        enum
        {
            color,
            string,
            uint,
            sint,
            address
        } tag;

        union
        {
            ::color::colors c;
            const char * s;
            uint64_t ui;
            int64_t si;
            void * a;
        };
    };

    enum
    {
        no = 0,
        yes = 1,
        committing = 2
    } _status = no;

    message _messages[256] = {};
    uint64_t _queue_size = 0;

    void _inc_size()
    {
        ++_queue_size;

        if (_queue_size >= 256)
        {
            screen::commit();
            screen::print("\nTransaction was too long; committed.\n");
            screen::transaction();
        }
    }
}

screen::kernel_console::kernel_console(terminal * term) : _terminal{term}, _owner{}, _count{}, _lock{}, _panicing{}
{
}

void screen::kernel_console::clear()
{
    _terminal->clear();
}

void screen::kernel_console::print(char c)
{
    _terminal->put_char(c);

    if (c != '\0')
    {
        outb(0x378, (unsigned char)c);
        outb(0x37a, 0x0c);
        outb(0x37a, 0x0d);
    }
}

void screen::kernel_console::print(const char * str)
{
    if (_status == yes)
    {
        _messages[_queue_size].tag = message::string;
        _messages[_queue_size].s = str;

        _inc_size();
    }

    else
    {
        while (*str)
        {
            outb(0x378, (unsigned char)*str);
            outb(0x37a, 0x0c);
            outb(0x37a, 0x0d);

            _terminal->put_char(*str++);

            if (_status == committing && *(str - 1) == '\n')
            {
                print(" - ");
            }
        }
    }
}

void screen::kernel_console::set_color(color::colors c)
{
    if (_status == yes)
    {
        _messages[_queue_size].tag = message::color;
        _messages[_queue_size].c = c;

        _inc_size();
    }

    else
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
    if (_status == yes)
    {
        _messages[_queue_size].tag = message::sint;
        _messages[_queue_size].si = i;

        _inc_size();
    }

    else
    {
        _print_int(i);
    }
}

void screen::kernel_console::print(int16_t i)
{
    if (_status == yes)
    {
        _messages[_queue_size].tag = message::sint;
        _messages[_queue_size].si = i;

        _inc_size();
    }

    else
    {
        _print_int(i);
    }
}

void screen::kernel_console::print(int32_t i)
{
    if (_status == yes)
    {
        _messages[_queue_size].tag = message::sint;
        _messages[_queue_size].si = i;

        _inc_size();
    }

    else
    {
        _print_int(i);
    }
}

void screen::kernel_console::print(int64_t i)
{
    if (_status == yes)
    {
        _messages[_queue_size].tag = message::sint;
        _messages[_queue_size].si = i;

        _inc_size();
    }

    else
    {
        _print_int(i);
    }
}

void screen::kernel_console::print(uint8_t i)
{
    if (_status == yes)
    {
        _messages[_queue_size].tag = message::uint;
        _messages[_queue_size].ui = i;

        _inc_size();
    }

    else
    {
        _print_int(i);
    }
}

void screen::kernel_console::print(uint16_t i)
{
    if (_status == yes)
    {
        _messages[_queue_size].tag = message::uint;
        _messages[_queue_size].ui = i;

        _inc_size();
    }

    else
    {
        _print_int(i);
    }
}

void screen::kernel_console::print(uint32_t i)
{
    if (_status == yes)
    {
        _messages[_queue_size].tag = message::uint;
        _messages[_queue_size].ui = i;

        _inc_size();
    }

    else
    {
        _print_int(i);
    }
}

void screen::kernel_console::print(uint64_t i)
{
    if (_status == yes)
    {
        _messages[_queue_size].tag = message::uint;
        _messages[_queue_size].ui = i;

        _inc_size();
    }

    else
    {
        _print_int(i);
    }
}

void screen::kernel_console::print(void * ptr)
{
    if (_status == yes)
    {
        _messages[_queue_size].tag = message::address;
        _messages[_queue_size].a = ptr;

        _inc_size();
    }

    else
    {
        print("0x");

        for (uint64_t i = 64; i > 0; i -= 4)
        {
            print("0123456789ABCDEF"[(((uint64_t)ptr) >> (i - 4)) & 0xF]);
        }
    }
}

void screen::kernel_console::transaction()
{
    commit();
    _status = yes;
}

void screen::kernel_console::commit()
{
    if (_status == yes)
    {
        _status = committing;

        for (uint64_t i = 0; i < _queue_size; ++i)
        {
            switch (_messages[i].tag)
            {
                case message::color:
                    set_color(_messages[i].c);
                    break;
                case message::string:
                    print(_messages[i].s);
                    break;
                case message::uint:
                    print(_messages[i].ui);
                    break;
                case message::sint:
                    print(_messages[i].si);
                    break;
                case message::address:
                    print(_messages[i].a);
            }
        }

        _status = no;
        _queue_size = 0;
    }
}

void screen::kernel_console::special(bool b)
{
    static decltype(_status) _old;
    static bool _first = true;

    if (_first)
    {
        _old = _status;
        _first = false;
    }

    if (b)
    {
        _old = _status;
        _status = no;
    }

    else
    {
        _status = _old;
    }
}

extern "C" void __lock(uint8_t *);
extern "C" void __unlock(uint8_t *);

void screen::kernel_console::lock()
{
    if (_panicing)
    {
        return;
    }

    uint64_t id = processor::current_core::id();

    __lock(&_lock);
    auto guard = make_scope_guard([&](){ __unlock(&_lock); });

    if (_owner == id)
    {
        ++_count;
    }

    else
    {
        while (_count)
        {
            __unlock(&_lock);
            asm volatile ("pause");
            __lock(&_lock);
        }

        _owner = id;
        ++_count;
    }
}

void screen::kernel_console::unlock()
{
    if (_panicing || !_count)
    {
        return;
    }

    __lock(&_lock);

    uint64_t id = processor::current_core::id();

    if (!_count)
    {
        PANIC("Tried to unlock not locked console");
    }

    if (_owner != id)
    {
        for (;;);
        PANIC("Tried to unlock console from wrong core");
    }

    --_count;

    __unlock(&_lock);
}

void screen::kernel_console::release()
{
    if (_panicing)
    {
        return;
    }

    __lock(&_lock);
    _panicing = 1;

    if (_count && _owner == processor::current_core::id())
    {
        _count = 0;
    }

    __unlock(&_lock);
}

#include "screen.h"
#include "console.h"
#include "../memory/memmap.h"

namespace screen
{
    console * output = nullptr;
}

void screen::initialize(screen::boot_mode * mode, void * font, memory::map & map)
{
    output = new console(mode, font, map);
}

void screen::line()
{
    output->put_char('\n');
}

void screen::print(const char * str)
{
    while (*str != 0)
    {
        output->put_char(*str++);
    }
}

void screen::print(char c)
{
    output->put_char(c);
}

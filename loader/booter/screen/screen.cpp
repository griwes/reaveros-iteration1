#include "screen.h"
#include "console.h"

namespace screen
{
    console * output = nullptr;
}

void screen::initialize(screen::boot_mode * mode, void * font)
{
    output = new console(mode, font);
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

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

}

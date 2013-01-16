#include <memory/memory.h>
#include <memory/x64paging.h>
#include <processor/processor.h>

namespace
{
    screen::mode _mode;
    memory::map_entry _map[512] = {};
    uint8_t _font[4096];
}

void memory::copy_bootloader_data(screen::mode *& video, memory::map_entry *& entries, uint64_t size)
{
    _mode = *video;
    memory::copy(entries, _map, size);
    memory::copy(_mode.font, _font, 4096);
    _mode.font = _font;
    
    video = &_mode;
    entries = _map;
    
    return;
}

void memory::initialize_paging()
{
    x64::pml4 * boot_vas = processor::get_cr3();
    
    (*boot_vas)[256] = (uint64_t)boot_vas;
    
    processor::reload_cr3();
}

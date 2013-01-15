#include <memory/memory.h>

namespace
{
    screen::mode _mode;
    memory::map_entry _map[512] = {};
}

void memory::copy_bootloader_data(screen::mode *& video, memory::map_entry *& entries, uint64_t size)
{
    memory::copy(video, &_mode);
    memory::copy(entries, _map, size);
    
    video = &_mode;
    entries = _map;
    
    return;
}


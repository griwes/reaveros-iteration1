#include <memory/memory.h>
#include <memory/x64paging.h>
#include <processor/processor.h>

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

void memory::initialize_paging()
{
    x64::pml4 * boot_vas = processor::get_cr3();
    
    (*boot_vas)[0] = (uint64_t)boot_vas;
    
    processor::reload_cr3();
}

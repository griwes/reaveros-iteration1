#ifndef __rose_loader_uefi_memory_h__
#define __rose_loader_uefi_memory_h__

#include <Uefi.h>

namespace Memory
{
    class MemoryMap;

    void Initialize(EFI_SYSTEM_TABLE *);
    MemoryMap * GetMemoryMap();
}

#endif
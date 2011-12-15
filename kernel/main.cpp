#include "hal/types.h"
#include "memory/utils.h"
#include "hal/memory.h"

extern int32 sbss, ebss;

extern "C" void KernelMain(int32 * pMemoryMap, int32 pMemoryMapSize)
{
    Memory::Zero((uint8 *)&sbss, &ebss - &sbss);
    
    Arch::Initialize();
    
    Memory::Init(&ebss, pMemoryMap, pMemoryMapSize);
}
#include "memory.h"

void Memory::Init(int32 placement, int32 * memmap, int32 memmapsize)
{
    int iEndOfUsedMemory = 0;
#ifdef __x86__
# include "../arch/x86/paging.h"
    iEndOfUsedMemory = Arch::x86::Paging::Init(placement, memmap, memmapsize);
#elif defined(__x64__)
# include "../arch/x64/paging.h"
    iEndOfUsedMemory = Arch::x64::Paging::Init(placement, memmap, memmapsize);
#endif
    
    Memory::Manager::Init(iEndOfUsedMemory, memmap, memmapsize);
    Memory::MoveStackToKernelSpace();
}
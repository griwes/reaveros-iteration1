#include "memory.h"

void Memory::Init(int32 placement, int32 * memmap, int32 memmapsize)
{
#ifdef __x86__
# include "../arch/x86/paging.h"
    Arch::x86::Paging::Init(placement, memmap, memmapsize);
#elif defined(__x64__)
    Arch::x64::Paging::Init(placement, memmap, memmapsize);
#endif
}
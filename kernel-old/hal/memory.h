#ifndef __kernel_hal_memory_h__
#define __kernel_hal_memory_h__

#include "types.h"

# ifdef __x86__

#  include "../arch/x86/paging.h"

# elif defined(__x86_64__)

#  include "../arch/x86_64/paging.h"

# endif

namespace Memory
{
    extern void Init(int32, int32 *, int32);
}

#endif
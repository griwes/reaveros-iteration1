#ifndef __kernel_memory_utils_h__
#define __kernel_memory_utils_h__

#include "../hal/types.h"

namespace Memory
{
    void Zero(uint8 * start, int32 size)
    {
        if (size % 4 == 0)
        {
            uint32 * a = (uint32 *)start;
            size /= 4;
            
            while (size-- > 0)
            {
                *a = 0;
                a++;
            }
        }
        
        else
        {
            while (size-- > 0)
            {
                *start = 0;
                start++;
            }
        }
    }
}

#endif
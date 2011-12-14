#ifndef __kernel_arch_x86_processor_h
#define __kernel_arch_x86_processor_h
#include "../../hal/types.h"

extern "C" int32 GetPAEBit();

namespace Arch
{
    namespace x86
    {
        namespace Processor
        {
            bool CheckPAE()
            {
                if (GetPAEBit() == 1)
                {
                    return true;
                }
                
                else
                {
                    return false;
                }
            }
        }
    }
}

#endif
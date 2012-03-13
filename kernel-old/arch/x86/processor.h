#ifndef __kernel_arch_x86_processor_h
#define __kernel_arch_x86_processor_h
#include "../../hal/types.h"
#include "paging.h"

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
            
            extern void ReloadPagingWithPAE(Arch::x86::PAEPaging::PageDirectoryPointerTableEntry *[4]);
            extern void LoadCR3(int32);
            
            void LoadCR3(int64 address)
            {
                LoadCR3((int32)address);
            }
        }
    }
}

#endif
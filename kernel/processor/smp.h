#ifndef __rose_kernel_processor_smp_h__
#define __rose_kernel_processor_smp_h__

#include "../types.h"

namespace Processor
{
    namespace SMP
    {
        class Environment
        {
        public:
            Environment();
            ~Environment();
        
            void BootCore(uint64);
            void SleepCore(uint64);
            void WakeCore(uint64);
        
            void IntCore(uint64, uint64);
        
            void BootCores();

            void PrintStatus();
        };
    }
}

#endif
#ifndef __rose_kernel_processor_core_h__
#define __rose_kernel_processor_core_h__

#include "../types.h"

namespace Processor
{
    class LAPIC
    {
    public:
        LAPIC();
        ~LAPIC();

        void SetupIDT();
    };
    
    namespace SMP
    {
        class Core
        {
        public:
            Core();
            ~Core();

            uint64 Id();

            void Wake();
            void Sleep();

            void EnableInterrupts();
            void DisableInterrupts();

            Processor::LAPIC * LAPIC();
        };
    }
}

#endif
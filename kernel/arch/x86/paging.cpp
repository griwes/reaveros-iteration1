#include "paging.h"
#include "processor.h"

// one note:
// paging is currently enabled, however up to 3GB+48MB it's almost identity mapped
// just substract 0xc0000000
void Arch::x86::Paging::Init(int32 placement, int32 * memmap, int32 memmapsize)
{
    placement += 4095;
    placement &= ~4095;
    
    // PAE paging
    if (Arch::x86::Processor::CheckPAE() == 1)
    {
        Arch::x86::PAEPaging::PageDirectoryPointerTable * pdpt = 
                    reinterpret_cast<Arch::x86::PAEPaging::PageDirectoryPointerTable *>(placement);
                    
        placement += sizeof(Arch::x86::PAEPaging::PageDirectoryPointerTable);
        placement += 4095;
        placement &= ~4095;
        
        for (int i = 0; i < 4; i++)
        {
            pdpt->pdirs[i] = reinterpret_cast<Arch::x86::PAEPaging::PageDirectory *>(placement);
            pdpt->dirs[i].address = (placement - 3 * 1024 * 1024 * 1024) >> 12;
            pdpt->dirs[i].present = (i == 3 || i == 0 ? 1 : 0); // currently, only stack (< 1GB) and kernel (> 3GB) are present...
            
            placement += sizeof(Arch::x86::PAEPaging::PageDirectory);
            placement += 4095;
            placement &= ~4095;
        }

        // this one is mithical function, said to move some part of kernel code
        // to identity mapped page, execute it there and then magically coming back
        // from there
        Arch::x86::Processor::ReloadPagingWithPAE(&(pdpt->dirs));
    }
    
    // no PAE
    else
    {
        
    }
}

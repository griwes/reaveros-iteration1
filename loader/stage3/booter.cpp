#include "booter.h"
#include "screen.h"
#include "physmem.h"
#include "initrd.h"

namespace Booter
{
    void Initialize(int pPlacementAddress)
    {
        PhysMemory::PlacementAddress = pPlacementAddress;
    }

    void Panic(char * sMessage)
    {
        if (Screen::kout != 0)
            Screen::kout->Print(sMessage);
        
        else
            for (;;) ;
    }   

    void SetupInitRD(int pInitrd)
    {
        void * initrd = (void *)pInitrd;
        
        
    }
    
    void LoadStorageDriver(int)
    {

    }

    void LoadDriver(char *)
    {
        
    }
    
    void LoadKernel()
    {
        
    }
    
    void ExecuteKernel(int)
    {
        
    }    
}
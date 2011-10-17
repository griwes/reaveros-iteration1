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
        
        // it's better to print a message, however if panic happened before kout
        // initialization... well, there is nothing that should fail
        else
            for (;;) ;
    }   

    void SetupInitRD(int pInitrd)
    {
        // cast the pointer to actual initrd type...
        InitRD::RamDisk * initrd = (InitRD::RamDisk *)pInitrd;
        
        
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
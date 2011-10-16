#include "booter.h"
#include "screen.h"
#include "physmem.h"

namespace Booter
{
    void Initialize(int pPlacementAddress)
    {
        PhysMemory::PlacementAddress = pPlacementAddress;
    }

    void Panic(char * sMessage)
    {
        Screen::kout->Print(sMessage);
    }   

    void LoadKernel()
    {
        
    }

    void ExecuteKernel(int)
    {
        
    }
}
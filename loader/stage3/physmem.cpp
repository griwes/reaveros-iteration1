#include "physmem.h"
#include "screen.h"
#include "booter.h"

namespace PhysMemory
{
    int PlacementAddress = 0;
}

void * PhysMemory::Manager::Place(int iSize)
{
    if (!PhysMemory::PlacementAddress)
    {
        if (Screen::kout != 0)
            Booter::Panic("FATAL ERROR: tried to place an object before initializing memory manager!");
        
        for (;;) ;
    }
    
    void * returned = (void *)PhysMemory::PlacementAddress;
    PhysMemory::PlacementAddress += iSize;
    
    return returned;
}
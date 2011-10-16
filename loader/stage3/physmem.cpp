#include "physmem.h"
#include "screen.h"
#include "booter.h"

void * PhysMemory::Manager::Place(int iSize)
{
    if (!PhysMemory::PlacementAddress)
    {
        Booter::Panic("");
    }
    
    void * returned = PhysMemory::PlacementAddress;
    PhysMemory::PlacementAddress += iSize;
    
    return returned;
}
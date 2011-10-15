#include "physmem.h"
#include "screen.h"
#include "bal.h"

void * PhysMemory::Manager::Place(int iSize)
{
    if (!PhysMemory::PlacementAddress)
    {
        BAL::Panic();
    }
    
    void * returned = PhysMemory::PlacementAddress;
    PhysMemory::PlacementAddress += iSize;
    
    return returned;
}
#include "booter.h"
#include "screen.h"
#include "physmem.h"

void Booter::Initialize(int pMemoryMap, int pPlacementAddress)
{
    PhysMemory::PlacementAddress = pPlacementAddress;
    
    Booter::DetectCPU();
}


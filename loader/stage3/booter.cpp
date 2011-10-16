#include "booter.h"
#include "screen.h"
#include "physmem.h"

void Booter::Initialize(int pPlacementAddress)
{
    PhysMemory::PlacementAddress = pPlacementAddress;
}


#include "physmem.h"
#include "screen.h"
#include "booter.h"

namespace PhysMemory
{
    int PlacementAddress = 0;
    
    void PrintMemoryMap(int pMemoryMap, int iMemRegCount)
    {
        unsigned int * map = (unsigned int *)pMemoryMap;

        Screen::kout->Print("\nPrinting memory map (base, length, type):\n");
        
        for (int i = 0; i < iMemRegCount; i++)
        {
            Screen::kout->m_eMode = Screen::iHex;
            Screen::kout->Print(*(map + 1));
            Screen::kout->Print(*map)->Print(", ");
            map += 2;
            Screen::kout->Print(*(map + 1));
            Screen::kout->Print(*map)->Print(", ");
            map += 2;
            Screen::kout->m_eMode = Screen::iDec;
            Screen::kout->Print(*map)->Print('\n');
            map += 2;
        }
    }
}

void * PhysMemory::Manager::Place(int iSize)
{
    // oh well, we haven't initialized memory...
    if (!PhysMemory::PlacementAddress)
    {
        for (;;) ;
    }
    
    // cast it to void *
    void * returned = (void *)PhysMemory::PlacementAddress;
    // and increment
    PhysMemory::PlacementAddress += iSize;
    
    if (PhysMemory::PlacementAddress > 0x800000)
    {
        Booter::Panic("\nRunned out of memory!");
    }
    
    // and return
    return returned;
}

void * PhysMemory::Manager::PlacePageAligned(int iSize)
{
    if (!PhysMemory::PlacementAddress)
    {
        for (;;) ;
    }
    
    int address = PhysMemory::PlacementAddress + 4095;
    address &= 0xfffff000;
    
    PhysMemory::PlacementAddress = address + iSize;
    
    if (PhysMemory::PlacementAddress > 0x800000)
    {
        Booter::Panic("\nRunned out of memory!");
    }
    
    return (void *)address;
}

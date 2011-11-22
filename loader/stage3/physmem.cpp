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
        // this if is never gonna be fulfilled...
        if (Screen::kout != 0)
            // but there is pretty error message!
            Booter::Panic("FATAL ERROR: tried to place an object before initializing memory manager!");
        
        for (;;) ;
    }
    
    // cast it to void *
    void * returned = (void *)PhysMemory::PlacementAddress;
    // and increment
    PhysMemory::PlacementAddress += iSize;
    
    // and return
    return returned;
}
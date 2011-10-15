#include "booter.h"
#include "screen.h"

extern "C" void __attribute__((cedcl)) booter_main(int pMemoryMap, int pPlacementAddress, int iBootdrive)
{
    Booter::Initialize(pMemoryMap, pPlacementAddress);
    Screen::Initialize(); // kout is now ready
    
    Screen::kout->Clear();
    Screen::kout << "ReaverOS Booter, v0.1\n";
    
    
    Booter::LoadStorageDriver(iBootdrive);
    Booter::LoadKernel();
    Booter::ExecuteKernel(pMemoryMap);
    
    // we should never reach this one, however...
    for (;;) ;
    
    return;
}
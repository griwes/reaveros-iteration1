#include "booter.h"
#include "screen.h"

extern "C" void __attribute__((cdecl)) booter_main(int pInitrd, int pMemoryMap, int pPlacementAddress, int iBootdrive)
{
    Booter::Initialize(pPlacementAddress);
    Screen::Initialize(); // kout is now ready
    
    Screen::kout->Clear();
    Screen::kout->Print("ReaverOS Booter, v0.1\n");
    
    //Booter::SetupInitRD(pInitrd);
    //Booter::LoadStorageDriver(iBootdrive);
    //Booter::LoadDriver("/init/ReaverFS.drv");
    Booter::LoadKernel();
    
    Screen::kout->Clear();
    Booter::ExecuteKernel(pMemoryMap);
    
    // we should never reach this one, however...
    for (;;) ;
    
    return;
}
#include "booter.h"
#include "screen.h"

// Booter almost-entry point
extern "C" void __attribute__((cdecl)) booter_main(int pInitrd, int pMemoryMap, int pPlacementAddress, int iBootdrive)
{
    // setup placement address
    Booter::Initialize(pPlacementAddress);
    // initialize kout
    Screen::Initialize(); // kout is now ready
    
    // clear screen and print welcome message
    Screen::kout->Clear();
    Screen::kout->Print("ReaverOS Booter, v0.1\n");
    
    // setup initrd (cpt obvious)
    Booter::SetupInitRD(pInitrd);
    // load storage driver (from initrd, based on bootdrive id
    Booter::LoadStorageDriver(iBootdrive);
    // load filesystem driver (from initrd)
    Booter::LoadDriver("/init/reaverfs.drv");
    // load kernel (from bootdrive)
    Booter::LoadKernel();
    
    //Screen::kout->Clear();
    Booter::ExecuteKernel(pMemoryMap);
    
    // we should never reach this one, however...
    for (;;) ;
    
    // for the sake of decreasing number of compilation warnings...
    return;
}
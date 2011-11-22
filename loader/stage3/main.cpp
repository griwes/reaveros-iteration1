#include "booter.h"
#include "screen.h"
#include "physmem.h"

// Booter almost-entry point
extern "C" void __attribute__((cdecl)) booter_main(int pInitrd, int pMemoryMap, int iMemRegCount, int pPlacementAddress, int iBootdrive)
{
    // setup placement address
    Booter::Initialize(pPlacementAddress);
    // initialize kout
    Screen::Initialize(); // kout is now ready
    
    // clear screen and print welcome message
    Screen::kout->Clear();
    Screen::kout->Print("ReaverOS Booter, v0.1\n");

    PhysMemory::PrintMemoryMap(pMemoryMap, iMemRegCount);
    
    // setup initrd (cpt obvious)
    Booter::SetupInitRD(pInitrd);
    // load storage driver (from initrd, based on bootdrive id
    Booter::LoadStorageDriver(iBootdrive);
    // load filesystem driver (from initrd)
    Booter::LoadDriver("/init/reaverfs.drv");
    // load kernel (from bootdrive)
    Booter::LoadKernel();
    
    // setup paging and, if CPU is 64bit and kernel header is marked as 64bit, long mode
    Booter::SetupKernelEnvironment();
    unsigned long long int timestamp = Booter::GetTimestamp();
    Booter::ExecuteKernel(pMemoryMap, iMemRegCount, timestamp);
    
    // we should never reach this one, however...
    for (;;) ;
    
    // for the sake of decreasing number of compilation warnings...
    return;
}
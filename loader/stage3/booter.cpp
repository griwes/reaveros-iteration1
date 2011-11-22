#include "booter.h"
#include "screen.h"
#include "physmem.h"
#include "initrd.h"

namespace Booter
{
    InitRD::InitRD * initrd = 0;
    
    void Initialize(int pPlacementAddress)
    {
        PhysMemory::PlacementAddress = pPlacementAddress;
    }

    void Panic(char * sMessage)
    {
        if (Screen::kout != 0)
            Screen::kout->Print(sMessage);
        
        // it's better to print a message, however if panic happened before kout
        // initialization... well, there is nothing that should fail
        else
            for (;;) ;
    }   

    void SetupInitRD(int pInitrd)
    {
        Booter::initrd = (InitRD::InitRD *)PhysMemory::Manager::Place(sizeof(InitRD::InitRD));
        Booter::initrd->Initialize(pInitrd);        
    }
    
    void LoadStorageDriver(int)
    {

    }

    void LoadDriver(char *)
    {
        
    }
    
    void LoadKernel()
    {
        
    }
    
    void SetupKernelEnvironment()
    {

    }
    
    void ExecuteKernel(int, int, Screen::VBEModeDescriptor *, unsigned long long int)
    {
        
    }    
    
    int GetTimestamp()
    {
        
    }
}
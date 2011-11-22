#ifndef __booter_h__
#define __booter_h__

#include "initrd.h"
#include "screen.h"

// namespace holding various general functions
namespace Booter
{
    // initializes memory "manager"
    extern void Initialize(int);
    
    // setups initrd structures
    extern void SetupInitRD(int);
    // loads floppy/(S)ATA driver based on boot device id
    extern void LoadStorageDriver(int);
    // loads driver from initrd
    extern void LoadDriver(char *);
    // loads kernel
    extern void LoadKernel();
    // jumps to kernel
    extern void ExecuteKernel(int, int, unsigned long long int);

    // setups paging and long mode, if required
    extern void SetupKernelEnvironment();
    
    // print panic message
    extern void Panic(char *);
    
    extern int GetTimestamp();
    
    extern InitRD::InitRD * initrd;
}

#endif
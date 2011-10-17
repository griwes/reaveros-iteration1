#ifndef __booter_h__
#define __booter_h__

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
    extern void ExecuteKernel(int);

    // print panic message
    extern void Panic(char *);
}

#endif
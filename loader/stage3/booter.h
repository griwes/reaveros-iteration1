#ifndef __booter_h__
#define __booter_h__

namespace Booter
{
    extern void Initialize(int);
    
    extern void SetupInitRD(int);
    extern void LoadStorageDriver(int);
    extern void LoadDriver(char *);
    extern void LoadKernel();
    extern void ExecuteKernel(int);

    extern void Panic(char *);
}

#endif
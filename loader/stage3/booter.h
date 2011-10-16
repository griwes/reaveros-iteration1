#ifndef __booter_h__
#define __booter_h__

namespace Booter
{
    void Panic(char *);
    
    void Initialize(int);
    bool Check64();
    
    void LoadStorageDriver(int);
    void LoadKernel();
    void ExecuteKernel(int);
}

#endif
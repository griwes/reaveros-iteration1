#ifndef __physmem_h__
#define __physmem_h__

namespace PhysMemory
{
    extern int PlacementAddress;
    
    class Manager
    {
    public:
        static void * Place(int iSize);
    };
}

#endif
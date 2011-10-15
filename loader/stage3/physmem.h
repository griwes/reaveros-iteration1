#ifndef __physmem_h__
#define __physmem_h__

namespace PhysMemory
{
    int PlacementAddress = 0;
    
    class Manager
    {
    public:
        static void * Place(int iSize);
    };
}

#endif
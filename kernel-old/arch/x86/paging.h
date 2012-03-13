#ifndef __kernel_arch_x86_paging_h__
#define __kernel_arch_x86_paging_h__
#include "../../hal/types.h"

namespace Arch
{
    namespace x86
    {
        namespace Paging
        {
            extern int Init(int32, int32 *, int32);
            
            struct PageEntry
            {
                int present : 1;
                int writeable : 1;
                int user : 1;
                int writethrough : 1;
                int cachedisable : 1;
                int accessed : 1;
                int dirty : 1;
                int ignored : 5;
                int address : 20;
            };
            
            struct PageTable
            {
                PageEntry entries[1024];
            };
            
            struct PageDirectoryEntry
            {
                int present : 1;
                int writeable : 1;
                int user : 1;
                int writethrough : 1;
                int cachedisable : 1;
                int accessed : 1;
                int unused : 6;
                int address : 20;
            };
            
            struct PageDirectory
            {
                PageDirectoryEntry tables[1024];
                PageTable * pTables[1024];
                int directory; // physical, self
            }; // sizeof <= 3 pages
        }
        
        namespace PAEPaging
        {
            struct PageEntry
            {
                int present : 1;
                int writeable : 1;
                int user : 1;
                int writethrough : 1;
                int cachedisable : 1;
                int accessed : 1;
                int dirty : 1;
                int zero : 1;
                int ignored : 4;
                int address : 40;
                int zero_ : 12;
            };
            
            struct PageTable
            {
                PageEntry entries[512];
            };
            
            struct PageDirectoryEntry
            {
                int present : 1;
                int writeable : 1;
                int user : 1;
                int writethrough : 1;
                int cachedisable : 1;
                int accessed : 1;
                int ignored : 1;
                int zero : 1;
                int ignored_ : 4;
                int address : 40;
                int reserved : 12;
            };
            
            struct PageDirectory
            {
                PageDirectoryEntry tables[512]; // physical
                PageTable * pTables[512]; // virtual
                int directory; // physical, self
            }; // sizeof <= 2 pages
            
            struct PageDirectoryPointerTableEntry
            {
                int present : 1;
                int reserved : 2;
                int writethrough : 1;
                int cachedisable : 1;
                int reserved_ : 4;
                int ignored : 3;
                int address : 40;
                int reserved__ : 12;
            };
            
            struct PageDirectoryPointerTable
            {
                PageDirectoryPointerTableEntry dirs[4];
                PageDirectory * pdirs[4];
                int pdptp; // physical, self
            };
        }
    }
}

#endif
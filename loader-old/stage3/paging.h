#ifndef __paging_h__
#define __paging_h__

namespace Paging
{
    struct PageTable
    {
        int Pages[1024];
    };
    
    struct PageDirectory
    {
        int Tables[1024];
    };
}

#endif
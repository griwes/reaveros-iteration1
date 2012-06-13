#ifndef __rose_kernel_lib_list_h__
#define __rose_kernel_lib_list_h__

#include "../types.h"

namespace Lib
{
    template<typename T>
    class List;
}

namespace
{
    template<typename T>
    class _list_iterator;
    
    template<typename T>
    class _list_element
    {
    public:
        typedef T Type;
        typedef _list_iterator<T> Iterator;
        typedef _list_element<T> Element;
        typedef Lib::List<T> List;
    };

    template<typename T>
    class _list_iterator
    {
    public:
        typedef T Type;
        typedef _list_iterator<T> Iterator;
        typedef _list_element<T> Element;
        typedef Lib::List<T> List;
    };
}

namespace Lib
{
    template<typename T>
    class List
    {
    public:
        typedef T Type;
        typedef _list_iterator<T> Iterator;
        typedef _list_element<T> Element;
        typedef List<T> List;

        List(bool);
        ~List();

        Insert(const T &);
        Remove(const T &);
        Remove(Iterator);
    };
}

#endif
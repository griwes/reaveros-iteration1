#ifndef __rose_kernel_lib_vector_h__
#ifndef __rose_kernel_lib_vector_h__

#include "../types.h"
#include "../memory/memory.h"

namespace Lib
{
    template<typename T>
    class Vector;
}

namespace
{
    template<typename T>
    class _vector_iterator
    {
    public:
        typedef T Type;
        typedef _vector_iterator<T> Iterator;
        typedef Lib::Vector<T> Vector;

        friend class Vector;
        
        _vector_iterator(Vector *, uint64);
        ~_vector_iterator();

        T & operator*();
        T * operator->();

        Iterator operator++();
        Iterator operator++(uint64);
        Iterator operator--();
        Iterator operator--(uint64);

        Iterator operator+(uint64);
        Iterator & operator+=(uint64);
        Iterator operator-(uint64);
        Iterator & operator+=(uint64);

        Iterator & operator=(const Iterator &);

        bool operator<(const Iterator &);
        bool operator>(const Iterator &);
        bool operator==(const Iterator &);
        bool operator!=(const Iterator &);
        bool operator<=(const Iterator &);
        bool operator>=(const Iterator &);

    private:
        Vector * m_pParent;
        uint64 m_iRevision;
        uint64 m_iIndex;
    };
}

namespace Lib
{
    // basic vector implementation
    template<typename T>
    class Vector
    {
    public:
        typedef T Type;
        typedef _vector_iterator<T> Iterator;
        typedef Lib::Vector<T> Vectort;

        friend class Iterator;

        Vector() : m_pBuffer(nullptr), m_iSize(0), m_iCapacity(0), m_iRevision(0)
        {
        }
        
        Vector(const Vectort & v)
        {
            *this = v;
        }

        ~Vector()
        {
            delete[] m_pBuffer;
        }

        
        Iterator Insert(const T & val)
        {
            if (m_iSize + 1 > m_iCapacity)
            {
                _realloc();
            }

            m_pBuffer[m_iSize] = val;
            m_iSize++;

            return Iterator(this, m_iSize - 1);
        }
        
        void Remove(Iterator it)
        {
            if (it.m_pParent != this || it.m_pRevision != this->m_pRevision)
            {
                return;
            }

            it->~T();
            m_iSize--;
        }
        
        void Remove(uint64 ind)
        {
            if (ind >= m_iSize)
            {
                return;
            }

            m_pBuffer[ind].~T();
            m_iSize--;
        }

        
        Vectort operator+(const T & e)
        {
            Vectort temp(*this);
            temp += e;
            return temp;
        }
        
        Vectort & operator+=(const T & e)
        {
            
        }
        
        Vectort operator+(const Vectort & v)
        {
            Vectort temp(*this);
            temp += v;
            return temp;
        }
        
        Vectort & operator+=(const Vectort &)
        {
            
        }

        Vectort & operator=(const Vectort & v)
        {
            delete[] m_pBuffer;

            m_pBuffer = new T[v.m_iSize];
            m_iSize = v.m_iSize;
            m_iCapacity = m_iSize;
            Memory::Copy(v.m_pBuffer, m_pBuffer, m_iSize);

            return *this;
        }

        Vectort & operator=(const T & e)
        {
            *this = Vectort(e);
        }

        
        Iterator Begin()
        {
            return Iterator(this, 0);
        }
        
        Iterator End()
        {
            return Iterator(nullptr, 0);
        }

        
        T * Data()
        {
            return m_pBuffer;
        }
        
        T & operator[](uint64 ind)
        {
            if (ind >= m_iSize)
            {
                // cause unexplainable page fault
                // hey, we rarely hit this code!
                *(uint8 *)0 = 0;
            }

            return m_pBuffer[ind];
        }

        
        uint64 Size()
        {
            return m_iSize;
        }
        
        uint64 Capacity()
        {
            return m_iCapacity;
        }
        
        void Shrink()
        {
            T * oldbuffer = m_pBuffer;
            m_pBuffer = new T[m_iSize];
            Memory::Copy(oldbuffer, m_pBuffer, m_iSize);
            delete[] oldbuffer;
        }

    private:
        T * m_pBuffer;
        uint64 m_iSize;
        uint64 m_iCapacity;
        uint64 m_iRevision;

        void _realloc()
        {
            
        }
    };
}

#endif
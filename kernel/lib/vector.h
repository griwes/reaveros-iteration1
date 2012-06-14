#ifndef __rose_kernel_lib_vector_h__
#define __rose_kernel_lib_vector_h__

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
        
        friend class Lib::Vector<T>;
        
        _vector_iterator(Vector * v, uint64 i)
        : m_pParent(v), m_iRevision(v->m_iRevision), m_iIndex(i)
        {
        }
        
        _vector_iterator(const Vector &);
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
        Iterator & operator-=(uint64);
        
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
        
        friend class _vector_iterator<T>;
        
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
            return Insert(val, Iterator(this, m_iSize - 1));
        }
        
        Iterator Insert(const T & val, Iterator it)
        {
            if (it.m_pParent != this || it.m_iRevision != m_iRevision)
            {
                return;
            }
            
            if (m_iSize + 1 > m_iCapacity)
            {
                _realloc();
            }
            
            if (it.m_iIndex == m_iSize - 1)
            {
                m_pBuffer[m_iSize] = val;
            }
            
            else
            {
                for (uint64 i = 0; i + it.m_iIndex < m_iSize; i++)
                {
                    Memory::Copy(m_pBuffer + m_iSize - i - 1, m_pBuffer + m_iSize - 1);
                }
                
                m_pBuffer[it.m_iIndex + 1] = val;
            }
            
            m_iSize++;
            
            return Iterator(this, it.m_iIndex + 1);
        }
        
        void Remove(Iterator it)
        {
            if (it.m_pParent != this || it.m_iRevision != m_iRevision)
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
            Insert(e);
        }
        
        Vectort operator+(const Vectort & v)
        {
            Vectort temp(*this);
            temp += v;
            return temp;
        }
        
        Vectort & operator+=(const Vectort & v)
        {
            if (v.m_iSize + m_iSize > m_iCapacity)
            {
                _realloc();
            }
            
            Memory::Copy(v.m_pBuffer, m_pBuffer + m_iSize, v.m_iSize);
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
            return Iterator(this, m_iCapacity);
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
            m_iCapacity = m_iSize;
            Memory::Copy(oldbuffer, m_pBuffer, m_iSize);
            delete[] oldbuffer;
            
            m_iRevision++;
        }
        
    private:
        T * m_pBuffer;
        uint64 m_iSize;
        uint64 m_iCapacity;
        uint64 m_iRevision;
        
        void _realloc()
        {
            T * oldbuffer = m_pBuffer;
            
            if (m_iCapacity != 0)
            {
                m_iCapacity *= 2;
            }
            
            else
            {
                m_iCapacity = 2;
            }
            
            m_pBuffer = new T[m_iCapacity];
            Memory::Copy(oldbuffer, m_pBuffer, m_iSize);
            delete[] oldbuffer;
            
            m_iRevision++;
        }
    };
}

#endif
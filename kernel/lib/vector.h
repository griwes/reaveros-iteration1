/**
 * ReaverOS
 * kernel/lib/vector.h
 * Simple container class.
 */

/**
 * Reaver Project OS, Rose License
 *
 * Copyright (C) 2011-2012 Reaver Project Team:
 * 1. Michał "Griwes" Dominiak
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation is required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 * Michał "Griwes" Dominiak
 *
 **/

// TODO: REWRITE THIS SHIT

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
        
        _vector_iterator(const Iterator & it)
            : m_pParent(it.m_pParent), m_iRevision(it.m_iRevision), m_iIndex(it.m_iIndex)
        {
        }
        
        ~_vector_iterator()
        {
        }

        T & operator*()
        {
            if (m_iRevision == m_pParent->m_iRevision && m_iIndex < m_pParent->m_iSize)
            {
                return m_pParent->m_pBuffer[m_iIndex];
            }

            PANIC("Tried to dereference invalid vector iterator.");
            return *(T *)0; // for the sake of dropping compiler warning
        }
        
        Iterator & operator++()
        {
            m_iIndex++;
            return *this;
        }
        
        Iterator operator++(int)
        {
            Iterator tmp(*this);
            m_iIndex++;
            return tmp;
        }

        Iterator & operator--()
        {
            m_iIndex--;
            return *this;
        }
        
        Iterator operator--(int)
        {
            Iterator tmp(*this);
            m_iIndex--;
            return tmp;
        }

        Iterator operator+(uint64 i)
        {
            return Iterator(m_pParent, m_iIndex + i);
        }
        
        Iterator & operator+=(uint64 i)
        {
            m_iIndex += i;
            return *this;
        }
        
        Iterator operator-(uint64 i)
        {
            return Iterator(m_pParent, m_iIndex - i);
        }
        
        Iterator & operator-=(uint64 i)
        {
            m_iIndex -= i;
            return *this;
        }

        Iterator & operator=(const Iterator & it)
        {
            m_iIndex = it.m_iIndex;
            m_pParent = it.m_pParent;
            m_iRevision = it.m_iRevision;
            return *this;
        }

        bool operator<(Iterator it)
        {
            if (m_pParent != it.m_pParent)
            {
                return m_pParent < it.m_pParent;
            }

            return m_iIndex < it.m_iIndex;
        }
        
        bool operator>(Iterator it)
        {
            return it < *this;
        }
        
        bool operator==(Iterator it)
        {
            return (!(*this < it) && !(it < *this));
        }
        
        bool operator!=(Iterator it)
        {
            return (*this < it || it < *this);
        }
        
        bool operator<=(Iterator it)
        {
            return (*this < it || *this == it);
        }
        
        bool operator>=(Iterator it)
        {
            return (*this > it || *this == it);
        }

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

        Iterator PushBack(const T & val)
        {
            if (m_iSize + 1 > m_iCapacity)
            {
                _realloc();
            }

            m_pBuffer[m_iSize++] = val;

            return Iterator(this, m_iSize - 1);
        }

        Iterator PushFront(const T & val)
        {
            if (m_iSize + 1 > m_iCapacity)
            {
                _realloc();
            }

            for (uint64 i = 0; i < m_iSize; i++)
            {
                Memory::Copy(m_pBuffer + m_iSize - i - 1, m_pBuffer + m_iSize);
            }

            m_iSize++;

            m_pBuffer[0] = val;

            return Begin();
        }

        Iterator Insert(const T & val)
        {
            return Insert(val, Iterator(this, m_iSize - 1));
        }
        
        Iterator Insert(const T & val, Iterator it)
        {
            if (it.m_pParent != this || it.m_iRevision != m_iRevision)
            {
                PANIC("Tried to insert at position represented by invalid iterator.");
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
                    Memory::Copy(m_pBuffer + m_iSize - i - 1, m_pBuffer + m_iSize);
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
            
            (*it).~T();
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
            return Iterator(this, m_iSize);
        }
        
        
        T * Data()
        {
            return m_pBuffer;
        }
        
        T & operator[](uint64 ind)
        {
            if (ind >= m_iSize)
            {
                PANIC("Out of bounds access.");
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
#ifndef __rose_kernel_lib_rangemap_h__
#define __rose_kernel_lib_rangemap_h__

#include "bstree.h"
#include "../types.h"

namespace Lib
{
    template<typename>
    class RangeMap;
    
    namespace _details
    {
        template<typename _type>
        class _range
        {
        public:
            typedef _range<_type> Range;
            
            _range()
            {
            }
            
            _range(_type b, _type e) : m_tBase(b), m_tEnd(e)
            {
            }
            
            ~_range()
            {
            }
            
            template<typename T>
            friend class RangeMap;
            
            _type Base()
            {
                return m_tBase;
            }
            
            _type End()
            {
                return m_tEnd;
            }
            
            bool operator<(Range r)
            {
                return (m_tBase < r.m_tBase && *this != r);
            }
            
            bool operator>(Range r)
            {
                return (m_tEnd > r.m_tEnd && *this != r);
            }
            
            bool operator<=(Range r)
            {
                return (*this < r || *this == r);
            }
            
            bool operator>=(Range r)
            {
                return (*this > r || *this == r);
            }
            
            // two following operators may look weird, but are for comparing in BinarySearchTree
            bool operator!=(Range r)
            {
                return !(*this == r);
            }
            
            bool operator==(Range r)
            {
                return m_tBase == r.m_tBase || m_tEnd == r.m_tEnd || (m_tBase > r.m_tBase && m_tBase < r.m_tEnd)
                || (m_tEnd < r.m_tEnd && m_tEnd > r.m_tBase);
            }
            
        private:
            _type m_tBase;
            _type m_tEnd;
        };
        
        template<typename _value>
        class _range_map_iterator
        {
        private:
            typedef Trees::_details::_bs_tree_iterator<_range<uint64>, _value> _tree_iterator;
            
        public:
            typedef RangeMap<_value> Map;
            typedef _range_map_iterator<_value> Iterator;
            typedef _details::_range<uint64> Range;
            
            friend class RangeMap;
            
            _range_map_iterator()
            {
            }
            
            _range_map_iterator(_tree_iterator it) : m_tTreeIter(it)
            {
            }
            
            ~_range_map_iterator()
            {
            }
            
            Range GetRange()
            {
                return m_tTreeIter.Index();
            }
            
            _value & Value()
            {
                return m_tTreeIter.Value();
            }
            
            Iterator & operator++()
            {
                ++m_tTreeIter;
                return *this;
            }
            
            Iterator operator++(int)
            {
                Iterator tmp(*this);
                ++m_tTreeIter;
                return tmp;
            }
            
            Iterator & operator--()
            {
                --m_tTreeIter;
                return *this;
            }
            
            Iterator operator--(int)
            {
                Iterator tmp(*this);
                --m_tTreeIter;
                return tmp;
            }
            
            bool operator==(Iterator it)
            {
                return (m_tTreeIter == it.m_tTreeIter);
            }
            
            bool operator!=(Iterator it)
            {
                return (m_tTreeIter != it.m_tTreeIter);
            }
            
        private:
            _tree_iterator m_tTreeIter;
        };
    }
    
    template<typename _value>
    class RangeMap
    {
    public:
        typedef RangeMap<_value> Map;
        typedef _details::_range_map_iterator<_value> Iterator;
        typedef _details::_range<uint64> Range;
        
        friend class _details::_range_map_iterator<_value>;
        
        RangeMap()
        {
        }
        
        ~RangeMap()
        {
        }
        
        Iterator Insert(uint64 begin, uint64 end, _value val)
        {
            return m_tTree.Insert(_details::_range<uint64>(begin, end), val);
        }
        
        bool Remove(uint64 v)
        {
            Iterator it = m_tTree.Get(v);
            if (it != m_tTree.End())
            {
                return m_tTree.Remove(it);
            }
            
            return false;
        }
        
        Iterator Get(uint64 val)
        {
            for (auto it = Iterator(m_tTree.Begin()); it != Iterator(m_tTree.End()); ++it)
            {
                if (val >= it.GetRange().Base() && val <= it.GetRange().End())
                {
                    return Iterator(it);
                }
            }
            
            return Iterator();
        }
        
        Iterator Begin()
        {
            return Iterator(m_tTree.Begin());
        }
        
        Iterator End()
        {
            return Iterator(m_tTree.End());
        }
        
        uint64 Size()
        {
            return m_tTree.Size();
        }
        
    private:
        typedef Trees::_details::_bs_tree_iterator<_details::_range<uint64>, _value> _tree_iterator;
        typedef Trees::BinarySearch<_details::_range<uint64>, _value> _tree;
        
        _tree m_tTree;
    };
}

#endif
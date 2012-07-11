/**
 * ReaverOS
 * kernel/lib/bstree.h
 * Binary search tree implementation.
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

#ifndef __rose_kernel_lib_bstree_h__
#define __rose_kernel_lib_bstree_h__

#include "../types.h"

namespace Lib
{
    namespace Trees
    {
        template<typename, typename>
        class BinarySearch;
        
        namespace _details
        {
            class _null_type
            {
            };
            
            template<typename, typename>
            class _bs_tree_iterator;
            
            template<typename _index, typename _value>
            class _bs_tree_node
            {
            public:
                typedef BinarySearch<_index, _value> Tree;
                typedef _bs_tree_iterator<_index, _value> Iterator;
                typedef _bs_tree_node<_index, _value> Node;
                
                friend class BinarySearch<_index, _value>;
                friend class _bs_tree_iterator<_index, _value>;
                
                _bs_tree_node(Tree * pTree, Node * pParent, _index tIndex, _value tValue = _value())
                : m_tIndex(tIndex), m_tValue(tValue), m_pTree(pTree), m_pParent(pParent),
                m_pLeft(nullptr), m_pRight(nullptr)
                {
                }
                
                ~_bs_tree_node()
                {
                    delete m_pLeft;
                    delete m_pRight;
                    
                    if (m_pParent)
                    {
                        if (this == m_pParent->m_pLeft)
                        {
                            m_pParent->m_pLeft = nullptr;
                        }
                        
                        else
                        {
                            m_pParent->m_pRight = nullptr;
                        }
                    }
                    
                    else if (m_pTree)
                    {
                        m_pTree->m_pRoot = nullptr;
                    }
                }

            private:
                _index m_tIndex;
                _value m_tValue;
                
                Tree * m_pTree;
                
                Node * m_pParent;
                
                Node * m_pLeft;
                Node * m_pRight;
            };
            
            template<typename _index, typename _value>
            class _bs_tree_iterator
            {
            public:
                typedef BinarySearch<_index, _value> Tree;
                typedef _bs_tree_iterator<_index, _value> Iterator;
                typedef _bs_tree_node<_index, _value> Node;
                
                friend class BinarySearch<_index, _value>;
                friend class _bs_tree_node<_index, _value>;
                
                _bs_tree_iterator() : m_pNode(nullptr)
                {
                }
                
                _bs_tree_iterator(Node * pNode) : m_pNode(pNode)
                {
                }
                
                _bs_tree_iterator(const Iterator & it) : m_pNode(it.m_pNode)
                {
                }
                
                _index Index()
                {
                    if (m_pNode)
                    {
                        return m_pNode->m_tIndex;
                    }
                    
                    else
                    {
                        PANIC("Tried to dereference invalid binary search tree iterator!");
                        
                        return *(_index *)0; // valid code, isn't it?
                    }
                }
                
                _value & Value()
                {
                    if (m_pNode)
                    {
                        return m_pNode->m_tValue;
                    }
                    
                    else
                    {
                        PANIC("Tried to dereference invalid binary search tree iterator!");
                        
                        return *(_value *)0; // we don't like compiler warnings
                    }
                }
                
                Iterator & operator++()
                {
                    Node * orig = m_pNode;
                    
                    if (m_pNode)
                    {
                        if (!m_pNode->m_pParent)
                        {
                            m_pNode = m_pNode->m_pRight;
                            
                            if (m_pNode)
                            {
                                while (m_pNode->m_pLeft)
                                {
                                    m_pNode = m_pNode->m_pLeft;
                                }
                            }
                        }
                        
                        else
                        {
                            if (m_pNode->m_pRight)
                            {
                                m_pNode = m_pNode->m_pRight;
                                
                                while (m_pNode->m_pLeft)
                                {
                                    m_pNode = m_pNode->m_pLeft;
                                }
                            }
                            
                            else
                            {
                                auto idx = m_pNode->m_tIndex;
                                
                                while (m_pNode && m_pNode->m_tIndex <= idx)
                                {
                                    m_pNode = m_pNode->m_pParent;
                                }
                            }
                        }
                    }
                    
                    return *this;
                }
                
                Iterator operator++(int)
                {
                    Iterator tmp(*this);
                    ++*this;
                    return tmp;
                }
                
                Iterator & operator--()
                {
                    Node * orig = m_pNode;
                    
                    if (m_pNode)
                    {
                        if (!m_pNode->m_pParent)
                        {
                            m_pNode = m_pNode->m_pLeft;
                            
                            if (m_pNode)
                            {
                                while (m_pNode->m_pRight)
                                {
                                    m_pNode = m_pNode->m_pRight;
                                }
                            }
                        }
                        
                        else
                        {
                            if (m_pNode->m_pLeft)
                            {
                                m_pNode = m_pNode->m_pLeft;
                                
                                while (m_pNode->m_pRight)
                                {
                                    m_pNode = m_pNode->m_pRight;
                                }
                            }
                            
                            else
                            {
                                auto idx = m_pNode->m_tIndex;
                                
                                while (m_pNode && m_pNode->m_tIndex <= idx)
                                {
                                    m_pNode = m_pNode->m_pParent;
                                }
                            }
                        }
                    }
                    
                    return *this;
                }
                
                Iterator operator--(int)
                {
                    Iterator tmp(*this);
                    --*this;
                    return tmp;
                }
                
                bool operator==(Iterator it)
                {
                    return !(it != *this);
                }
                
                bool operator!=(Iterator it)
                {
                    if (m_pNode != nullptr && it.m_pNode != nullptr)
                    {
                        if (m_pNode->m_pTree != it.m_pNode->m_pTree)
                        {
                            return true;
                        }
                        
                        else
                        {
                            return m_pNode != it.m_pNode;
                        }
                    }
                    
                    else
                    {
                        return (bool)m_pNode;
                    }
                }
                
            private:
                Node * m_pNode;
            };
        }
        
        template<typename _index, typename _value = _details::_null_type>
        class BinarySearch
        {
        public:
            typedef BinarySearch<_index, _value> Tree;
            typedef _details::_bs_tree_iterator<_index, _value> Iterator;
            typedef _details::_bs_tree_node<_index, _value> Node;
            
            friend class _details::_bs_tree_node<_index, _value>;
            
            BinarySearch()
            : m_pRoot(nullptr), m_iSize(0)
            {
            }
            
            ~BinarySearch()
            {
                delete m_pRoot;
            }
            
            Iterator Insert(_index tIndex, _value tValue = _value())
            {
                if (!m_pRoot)
                {
                    m_pRoot = new Node(this, nullptr, tIndex, tValue);
                    return Iterator(m_pRoot);
                }
                
                Node * current = m_pRoot;
                
                while (true)
                {
                    if (current->m_tIndex == tIndex)
                    {
                        return End();
                    }
                    
                    if (current->m_tIndex > tIndex)
                    {
                        if (!current->m_pLeft)
                        {
                            current->m_pLeft = new Node(this, current, tIndex, tValue);
                            m_iSize++;
                            return Iterator(current->m_pLeft);
                        }
                        
                        current = current->m_pLeft;
                    }
                    
                    else
                    {
                        if (!current->m_pRight)
                        {
                            current->m_pRight = new Node(this, current, tIndex, tValue);
                            m_iSize++;
                            return Iterator(current->m_pRight);
                        }
                        
                        current = current->m_pRight;
                    }
                }
            }
            
            bool Remove(Iterator it)
            {
                return Remove(it.Index());
            }
            
            bool Remove(_index tIndex)
            {
                Node * current = m_pRoot;
                
                if (!current)
                {
                    return true;
                }
                
                while (current)
                {
                    if (current->m_tIndex == tIndex)
                    {
                        if (!current->m_pLeft && !current->m_pRight)
                        {
                            delete current;
                            
                            m_iSize--;
                            
                            return true;
                        }
                        
                        else if (current == m_pRoot && !current->m_pLeft)
                        {
                            m_pRoot = current->m_pRight;
                            current->m_pRight->m_pParent = nullptr;
                            current->m_pRight = nullptr;
                            delete current;
                            
                            m_iSize--;
                            
                            return true;
                        }
                        
                        else if (current == m_pRoot && !current->m_pRight)
                        {
                            m_pRoot = current->m_pLeft;
                            current->m_pLeft->m_pParent = nullptr;
                            current->m_pLeft = nullptr;
                            delete current;
                            
                            m_iSize--;
                            
                            return true;
                        }
                        
                        else if (!current->m_pLeft)
                        {
                            Node * pRightLeaf = current->m_pRight;
                            
                            pRightLeaf->m_pParent = current->m_pParent;
                            
                            if (current == current->m_pParent->m_pLeft)
                            {
                                current->m_pParent->m_pLeft = pRightLeaf;
                            }
                            
                            else
                            {
                                current->m_pParent->m_pRight = pRightLeaf;
                            }
                            
                            current->m_pParent = nullptr;
                            current->m_pTree = nullptr;
                            
                            current->m_pRight = nullptr;
                            
                            delete current;
                            
                            m_iSize;
                            
                            return true;
                        }
                        
                        else if (!current->m_pRight)
                        {
                            Node * pLeftLeaf = current->m_pLeft;
                            
                            pLeftLeaf->m_pParent = current->m_pParent;
                            
                            if (current == current->m_pParent->m_pLeft)
                            {
                                current->m_pParent->m_pLeft = pLeftLeaf;
                            }
                            
                            else
                            {
                                current->m_pParent->m_pRight = pLeftLeaf;
                            }
                            
                            current->m_pParent = nullptr;
                            current->m_pTree = nullptr;
                            
                            current->m_pLeft = nullptr;
                            
                            delete current;
                            
                            m_iSize--;
                            
                            return true;
                        }
                        
                        else if (current == m_pRoot)
                        {
                            m_pRoot = current->m_pLeft;
                            
                            current->m_pParent = nullptr;
                            current->m_pTree = nullptr;
                            
                            if (m_pRoot->m_tIndex > current->m_pRight->m_tIndex)
                            {
                                m_pRoot->m_pRight = current->m_pRight;
                            }
                            
                            else
                            {
                                m_pRoot->m_pLeft = current->m_pRight;
                            }
                            
                            current->m_pLeft = nullptr;
                            current->m_pRight = nullptr;
                            
                            delete current;
                            
                            m_iSize--;
                            
                            return true;
                        }
                        
                        else
                        {
                            if (current == current->m_pParent->m_pLeft)
                            {
                                current->m_pParent->m_pLeft = current->m_pLeft;
                            }
                            
                            else
                            {
                                current->m_pParent->m_pRight = current->m_pLeft;
                            }
                            
                            if (current->m_pLeft->m_tIndex > current->m_pRight->m_tIndex)
                            {
                                current->m_pLeft->m_pLeft = current->m_pRight;
                            }
                            
                            else
                            {
                                current->m_pLeft->m_pRight = current->m_pRight;
                            }
                            
                            current->m_pParent = nullptr;
                            current->m_pTree = nullptr;
                            
                            current->m_pLeft = nullptr;
                            current->m_pRight = nullptr;
                            
                            m_iSize--;
                            
                            delete current;
                            
                            return true;
                        }
                    }
                    
                    else if (current->m_tIndex > tIndex)
                    {
                        current = current->m_pLeft;
                    }
                    
                    else
                    {
                        current = current->m_pRight;
                    }
                }
                
                return false;
            }
            
            Iterator Get(_index tIndex)
            {
                Node * current = m_pRoot;
                
                while (current)
                {
                    if (current->m_tIndex == tIndex)
                    {
                        return Iterator(current);
                    }
                    
                    if (current->m_tIndex > tIndex)
                    {
                        current = current->m_pLeft;
                    }
                    
                    else
                    {
                        current = current->m_pRight;
                    }
                }
                
                return End();
            }
            
            Iterator Begin()
            {
                Node * current = m_pRoot;
                
                while (true)
                {
                    if (!current)
                    {
                        return End();
                    }
                    
                    if (current->m_pLeft)
                    {
                        current = current->m_pLeft;
                    }
                    
                    else
                    {
                        return Iterator(current);
                    }
                }
            }
            
            Iterator End()
            {
                return Iterator();
            }
            
            uint64 Size()
            {
                return m_iSize;
            }
            
        private:
            Node * m_pRoot;
            uint64 m_iSize;
        };
    }
}

#endif
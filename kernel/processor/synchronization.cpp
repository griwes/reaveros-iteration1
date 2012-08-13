/**
 * ReaverOS
 * kernel/processor/synchronization.cpp
 * Basic synchronization structures.
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

#include "synchronization.h"
#include "../scheduler/scheduler.h"

extern "C"
{
    void _lock(uint8 *);
    void _unlock(uint8 *);
}

Processor::Spinlock::Spinlock() : m_iLock(0)
{
}

Processor::Spinlock::~Spinlock()
{
}

void Processor::Spinlock::Lock()
{
    _lock(&this->m_iLock);
}

void Processor::Spinlock::Unlock()
{
    _unlock(&this->m_iLock);
}

Processor::Corelock::Corelock() : m_pCore(nullptr), m_iCount(0)
{
}

Processor::Corelock::~Corelock()
{
}

void Processor::Corelock::Lock()
{
    m_internal.Lock();
    
    Processor::SMP::Core * current = Scheduler::GetCurrentCore();
    
    if (!m_pCore)
    {
        m_pCore = current;
    }
    
    if (current == m_pCore)
    {
        m_iCount++;
    }
    
    else
    {
        m_internal.Unlock();
        
        while (m_iCount || m_pCore == current)
        {
            __asm volatile ("pause" ::: "memory");
        }
        
        m_internal.Lock();
        
        m_pCore = current;
        m_iCount++;
    }
    
    m_internal.Unlock();
}

void Processor::Corelock::Unlock()
{
    m_internal.Lock();
    
    Processor::SMP::Core * current = Scheduler::GetCurrentCore();
    
    if (current == m_pCore)
    {
        m_iCount--;
        
        if (!m_iCount)
        {
            m_pCore = nullptr;
        }
    }
    
    m_internal.Unlock();
}

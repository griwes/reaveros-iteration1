/**
 * ReaverOS
 * utils/builder/builderthread.h
 * Builder thread base class.
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
 * including commercial applications, adn to alter it and redistribute it
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

#ifndef _rose_utils_builder_builderthread_h_
#define _rose_utils_builder_builderthread_h_

#include <vector>
#include <string>
#include <mutex>
#include <thread>

namespace Rose
{
    namespace Utils
    {
        class BuilderThread
        {
        public:
            BuilderThread() {}
            virtual ~BuilderThread() = 0;
            virtual int Run();
            
            void Lock();
            void Unlock();
            
        protected:
            std::mutex m_lock;
            std::thread m_thread;
        };
    }
}

#endif
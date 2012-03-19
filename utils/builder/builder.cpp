/**
 * ReaverOS
 * utils/builder/builder.cpp
 * Builder super class implementation.
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

#include "builder.h"
#include "directorythread.h"

Rose::Utils::Builder::Builder()
{
}

Rose::Utils::Builder::~Builder()
{
}

int Rose::Utils::Builder::Run(std::string arg)
{    
    auto path = boost::filesystem::path(boost::filesystem::current_path().string() + arg);
 
    boost::filesystem::create_directory(path.string() + "/build");
    
    this->_create_directory_thread(path);
    
    std::vector<boost::filesystem::path> v;
    std::copy(boost::filesystem::directory_iterator(path), boost::filesystem::directory_iterator(),
              std::back_inserter(v));
    
    for (auto it = v.begin(); it != v.end(); it++)
    {
        this->_create_directory_thread(*it);
    }
}

void Rose::Utils::Builder::_create_directory_thread(boost::filesystem3::path path)
{
    if (!boost::filesystem::is_directory(path))
    {
        return;
    }
    
    this->m_mThreads.insert(std::make_pair("directory_thread:" + path.string(), new Rose::Utils::DirectoryThread(path)));
    this->m_mThreads[std::string("directory_thread" + path.string())]->Run();
}

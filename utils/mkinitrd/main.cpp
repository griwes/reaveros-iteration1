/**
 * ReaverOS
 * utils/mkinitrd/main.cpp
 * InitRD creator.
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

#include <iostream>
#include <fstream>
#include <chrono>
#include <map>
#include <string>
#include <cstring> // I hate this one

void Zero(char * buffer, int size)
{
    for (int i = 0; i < size; i++)
    {
        buffer[i] = 0;
    }

    return;
}

// filesize, but in 512 byte sectors
int Filesize(std::fstream * pFile)
{
    pFile->seekg(0, std::fstream::end);
    int tmp = pFile->tellg();
    pFile->seekg(0);

    return tmp;
}

int main(int argc, char ** argv)
{
    if (argc < 3)
    {
        std::cout << "ReaverOS InitRD creator, version 0.2" << std::endl << std::endl;
        std::cout << "Usage:" << std::endl;
        std::cout << "./mkinitrd <output> <list file> [more list files]" << std::endl;
        std::cout << "Format of list files:" << std::endl;
        std::cout << "<original file path>" << std::endl;
        std::cout << "<destination file path>" << std::endl;
        std::cout << "Yes, two lines.)" << std::endl;

        return 1;
    }

    std::map<std::fstream *, std::string> mFiles;

    for (int i = 2; i < argc; i++)
    {
        std::fstream input(argv[i], std::fstream::in);

        if (!input.good())
        {
            std::cout << "Couldn't open list file " << argv[i] << "." << std::endl;

            return 2;
        }

        std::string orig, dest;
        input >> orig >> dest;

        if (input.fail())
        {
            std::cout << "Syntax error in file " << argv[i] << "." << std::endl;

            return 3;
        }

        if (dest.size() > 508)
        {
            std::cout << "Destination filename " << dest << " in list file " << argv[i]
                            << " is too long." << std::endl;

            return 4;
        }

        std::fstream * file = new std::fstream(orig.c_str(), std::fstream::in | std::fstream::binary);

        if (!file->good())
        {
            std::cout << "Couldn't open file " << orig << " from list file " << argv[i]
                            << "." << std::endl;

            return 5;
        }

        mFiles.insert(std::make_pair(file, dest));
    }

    std::fstream output(argv[1], std::fstream::out | std::fstream::binary);

    if (!output.good())
    {
        std::cout << "Couldn't open output file." << std::endl;

        return 6;
    }

    char * buffer = new char[512];

    *((int32_t *)buffer) = mFiles.size();
    *((uint32_t *)(buffer + 4) = std::chrono::system_clock::now().time_since_epoch().count() / 1000000;
    output.write(buffer, 512);

    for (auto it = mFiles.begin(); it != mFiles.end(); it++)
    {
        std::fstream * file = (*it).first();
        std::string dest = (*it).second();

        Zero(buffer, 512);

        std::memcpy(buffer, dest.c_str(), 508);

        int size = Filesize(file);
        int32_t * destsize = (int32_t *)(buffer + 508);
        (*destsize) = size;

        output.write(buffer, 512);

        while (!file->eof())
        {
            Zero(buffer, 512);
            file->read(buffer, 512);
            output.write(buffer, 512);
        }

        file->close();
        delete file;
    }

    std::cout << "InitRD created successfully." << std::endl;

    delete[] buffer;

    return 0;
}
/**
 * ReaverOS
 * utils/mkrfloppy/main.cpp
 * ReaverOS bootable ReaverFS image creator.
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

void Usage()
{
    std::cout << "ReaverFS bootfloppy creator, version 0.2" << std::endl << std::endl;
    std::cout << "Usage:" << std::endl;
    std::cout << "./mkrfloppy <output> <1st stage> <2nd stage> <booter> <kernel> <initrd>" << std::endl;

    return;
}

void Zero(char * buffer, int size)
{
    for (int i = 0; i < size; i++)
    {
        buffer[i] = 0;
    }
    
    return;
}

int main(int argc, char ** argv)
{
    if (argc != 7)
    {
        Usage();
        return 1;
    }
    
    std::fstream output(argv[1], std::fstream::out | std::fstream::trunc | std::fstream::binary);
    if (!output.good())
    {
        std::cout << "Couldn't open output file." << std::endl;
        return 2;
    }
    
    std::fstream stage1(argv[2], std::fstream::in | std::fstream::binary);
    if (!stage1.good())
    {
        std::cout << "Couldn't open stage 1 file." << std::endl;
        return 3;
    }
    
    std::fstream stage2(argv[3], std::fstream::in | std::fstream::binary);
    if (!stage2.good())
    {
        std::cout << "Couldn't open stage 2 file." << std::endl;
        return 4;
    }
    
    std::fstream booter(argv[4], std::fstream::in | std::fstream::binary);
    if (!booter.good())
    {
        std::cout << "Couldn't open booter file." << std::endl;
        return 5;
    }
    
    std::fstream kernel;
    kernel.open(argv[5], std::fstream::in | std::fstream::binary);
    if (!kernel.good())
    {
        std::cout << "Couldn't open kernel file." << std::endl;
        return 6;
    }

    std::fstream initrd;
    initrd.open(argv[6], std::fstream::in | std::fstream::binary);
    if (!initrd.good())
    {
        std::cout << "Couldn't open intird file." << std::endl;
        return 7;
    }

    char * buffer = new char[512];
    Zero(buffer, 512);
    
    stage1.read(buffer, 512);
    output.write(buffer, 512);
    Zero(buffer, 512);
    
    stage1.close();
    
    uint16_t counter = 0;
    while (!stage2.eof())
    {
        stage2.read(buffer, 512);
        output.write(buffer, 512);
        Zero(buffer, 512);
        counter++;
    }
    stage2.close();
    
    int pos = output.tellp();
    // write stage 2 size into stage 1 header
    output.seekp(22);
    output.write(reinterpret_cast<char *>(&counter), sizeof(counter));
    // write stage 2 size into stage 2 header
    output.seekp(512 + 8);
    output.write(reinterpret_cast<char *>(&counter), sizeof(counter));
    output.seekp(pos);
    
    uint16_t booter_counter = 0;
    while (!booter.eof())
    {
        booter.read(buffer, 512);
        output.write(buffer, 512);
        Zero(buffer, 512);
        booter_counter++;
    }
    booter.close();
    
    pos = output.tellp();
    // write size of booter into stage 2 header
    output.seekp(512 + 10);
    output.write(reinterpret_cast<char *>(&booter_counter), sizeof(booter_counter));
    output.seekp(pos);

    uint16_t kernel_counter = 0;
    while (!kernel.eof())
    {
        kernel.read(buffer, 512);
        output.write(buffer, 512);
        Zero(buffer, 512);
        kernel_counter++;
    }

    pos = output.tellp();
    // write size of kernel into stage 2 header
    output.seekp(512 + 12);
    output.write(reinterpret_cast<char *>(&kernel_counter), sizeof(kernel_counter));
    output.seekp(pos);

    uint16_t initrd_counter = 0;
    while (!initrd.eof())
    {
        initrd.read(buffer, 512);
        output.write(buffer, 512);
        Zero(buffer, 512);
        initrd_counter++;
    }

    // write size of initrd into stage 2 header
    output.seekp(512 + 14);
    output.write(reinterpret_cast<char *>(&initrd_counter), sizeof(initrd_counter));

    output.seekp(20);
    uint16_t sum = initrd_counter + kernel_counter + booter_counter + counter + 1;
    output.write(reinterpret_cast<char *>(&sum), sizeof(sum));
    
    output.close();
    
    std::cout << "Boot image created successfully." << std::endl;
    
    return 0;
}
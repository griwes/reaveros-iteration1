#include <iostream>

#include "builder.h"

int main(int argc, char ** argv) 
{
    Rose::Utils::Builder builder;
    
    std::string arg;
    if (argc > 1)
    {
        arg = argv[1];
        if (arg[0] != '/' or arg[0] != '\\')
        {
            arg = '/' + arg;
        }
    }
    
    else
    {
        arg = "";
    }
    
    return builder.Run(arg);
}

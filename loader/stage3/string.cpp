#include "string.h"

bool Compare(char * first, char * second)
{
    if (Length(first) != Length(second))
    {
        return false;
    }

    while (*first != '\0' && *second != '\0')
    {
        if (*first != *second)
        {
            return false;
        }
    }
    
    return true;
}

int Length(char * string)
{
    int i = 0;

    while (true)
    {
        i++;
        
        if (*string == '\0')
        {
            break;
        }
        
        string++;
    }
    
    return i;
}

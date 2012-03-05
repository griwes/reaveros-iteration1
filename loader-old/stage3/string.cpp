#include "string.h"
#include "screen.h"

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
        
        // oh well
        // I spent 30 minutes on finding bug
        // two following lines were missing
        // this bug was epic, don't you think?
        first++;
        second++;
    }
    
    if (*first != *second)
    {
        return false;
    }
    
    return true;
}

bool Compare(const char * first, char * second)
{
    return Compare(const_cast<char *>(first), second);
}

int Length(const char * string)
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

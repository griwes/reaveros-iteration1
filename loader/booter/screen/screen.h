#pragma once

#include "console.h"

namespace screen
{
    class boot_mode;
    
    extern console * output;
    
    void initialize(boot_mode *, void *, memory::map &);
    
    void print(const char *);
    void print(char);
    
    void line();
    
    template<typename T>
    void print(const T & a);
    
    template<typename First, typename... T>
    void print(const First & first, const T &... rest)
    {
        print(first);
        print(rest...);
    }
    
    template<typename... T>
    void printl(const T &... a)
    {
        print(a...);
        line();
    }
    
    inline void printf(const char * str)
    {
        print(str);
    }
    
    template<typename T>
    void printf(const char *& str, const T & param)
    {
        while (*str != 0)
        {
            if (*str != '%')
            {
                output->put_char(*str++);
            }
            
            else
            {
                switch (*++str)
                {
                    case '0':
                        switch (*++str)
                        {
                            case '1':
                                switch (*++str)
                                {
                                    case '6':
                                        switch (*++str)
                                        {
                                            case 'x':
                                                for (int32_t i = sizeof(T) * 8 - 4; i >= 0; i -= 4)
                                                {
                                                    print("0123456789ABCDEF"[(param >> i) & 0xf]);
                                                }
                                                
                                                ++str;
                                                
                                                return;
                                        }
                                }
                        }
                }
            }
        }
    }
    
    template<typename First, typename... T>
    void printf(const char * str, const First & first, const T &... rest)
    {
        printf(str, first);
        
        if (*str == 0)
        {
            return;
        }
        
        printf(str, rest...);
        
        if (*str == 0)
        {
            return; 
        }
        
        printf(str);
    }
    
    template<typename... T>
    void printfl(const char * s, T &... a)
    {
        printf(s, a...);
        line();
    }
}

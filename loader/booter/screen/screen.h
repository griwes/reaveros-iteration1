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

#pragma once

#include <screen/console.h>

template<bool b, typename T = void>
struct enable_if
{
};

template<typename T>
struct enable_if<true, T>
{
    typedef T type;
};

struct true_type
{
    static const bool value = true;
};

struct false_type
{
    static const bool value = false;
};

template<typename T>
struct is_integral : false_type {};

#define helper(x) template<> struct is_integral<x> : true_type {}

helper(int8_t);
helper(int16_t);
helper(int32_t);
helper(int64_t);
helper(uint8_t);
helper(uint16_t);
helper(uint32_t);
helper(uint64_t);

#undef helper

namespace screen
{
    class boot_mode;
    
    extern console * output;
    
    void initialize(boot_mode *, void *);
    
    void print(const char *);
    void print(char);
    
    void line();
    
    template<typename T>
    void print_impl(const T &);
    
    template<typename T>
    typename enable_if<!is_integral<T>::value>::type print(const T & a)
    {
        print_impl(a);
    }
        
    template<typename T>
    typename enable_if<is_integral<T>::value>::type print(const T & a)
    {        
        if (a == 0)
        {
            output->put_char('0');
            return;
        }
        
        else if (a < 0)
        {
            output->put_char('-');
        }
        
        T div = a / 10;
        T mod = a % 10;
        
        if (div != 0)
        {
            print(div);
        }
        
        output->put_char('0' + mod);
    }
    
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

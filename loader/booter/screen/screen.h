#pragma once

namespace screen
{
    class boot_mode;
    class console;
    
    void initialize(boot_mode *, void *);
    
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
    
    template<typename First, typename... T>
    void printf(const char * str, const First & first, const T &... rest);
    
    template<typename... T>
    void printfl(const char * s, T &... a)
    {
        printf(s, a...);
        line();
    }
    
    extern console * output;
}

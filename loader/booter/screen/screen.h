#pragma once

namespace screen
{
    class boot_mode;
    class console;
    
    void initialize(boot_mode *, void *);
    
    void printl(const char *);
    void print(const char *);
    void printl(char);
    void print(char);
    
    void line();
    
    template<typename... T>
    void print(const T &... a);
    
    template<typename... T>
    void printl(const T &... a)
    {
        print(a...);
        line();
    }
    
    template<typename... T>
    void printf(const char *, T &... a);
    
    template<typename... T>
    void printfl(const char * s, T &... a)
    {
        printf(s, a...);
        line();
    }
    
    template<typename T>
    void print(const T &);
    
    extern console * output;
}

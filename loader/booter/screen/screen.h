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
    
    template<typename... T>
    void printl(const T &... a);
    
    template<typename... T>
    void print(const T &... a);
    
    template<typename... T>
    void printf(const char *, T &... a);
    
    template<typename... T>
    void printfl(const char *, T &... a);
    
    template<typename T>
    void print(const T &);
    
    void line();
    
    extern console * output;
}

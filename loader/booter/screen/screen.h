#ifndef __rose_loader_booter_screen_h__
#define __rose_loader_booter_screen_h__

namespace screen
{
    class boot_mode_t;
    
    void initialize(boot_mode_t *, void *);
    
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
}

#endif

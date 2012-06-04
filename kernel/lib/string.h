#ifndef __rose_kernel_lib_string_h__
#define __rose_kernel_lib_string_h__

#include "../types.h"

// TODO: add UTF-8 support

namespace Lib
{
    inline uint64 Strlen(const char * s)
    {
        uint64 len = 0;

        while (*s != 0)
        {
            len++;
            s++;
        }

        return len;
    }

    class String
    {
    public:
        String();
        String(const char *);
        String(const String &);

        uint64 Length() const;
        const char * Buffer() const;
        
        String operator+(const String &);
        String & operator+=(const String &);
        friend String operator+(const char *, const String &);
        String & operator=(const String &);

    private:
        char * m_pData;
        uint64 m_iLength;
    };
}

#endif
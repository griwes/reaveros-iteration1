#ifndef __rose_kernel_lib_string_h__
#define __rose_kernel_lib_string_h__

class String
{
public:
    static String MakeConst(const char *);

    String();
    String(const char *);
};

#endif
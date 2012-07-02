#ifndef __rose_loader_uefi_screen_h__
#define __rose_loader_uefi_screen_h__

#include <Uefi.h>

namespace Screen
{
    class Console;
    class VideoMode;

    extern Console * uout;
    const char * nl = L"\n\r";
    
    void Initialize(EFI_SYSTEM_TABLE *);
    VideoMode * SelectVideoMode();
}

#endif
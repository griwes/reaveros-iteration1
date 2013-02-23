#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>

#include "memory.h"
#include "screen.h"

EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE * SystemTable)
{
    Memory::Initialize(SystemTable);
    Screen::Initialize(SystemTable);
    using Screen::uout;
    using Screen::nl;

    uout->Clear();

    *uout << L"ReaverOS UEFI Bootloader 0.1" << nl;
    *uout << L"Copyright (c) 2012 Reaver Development Team" << nl << nl;

    Memory::MemoryMap * pMemoryMap = Memory::GetMemoryMap();
    Screen::VideoMode * pVideoMode = Screen::SelectVideoMode();

    __asm__ __volatile__("cli\n hlt");

    return EFI_SUCCESS;
}

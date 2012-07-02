#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiApplicationEntryPoint.h>

EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE * SystemTable)
{
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut);
    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"ReaverOS UEFI Bootloader 0.1\r\n\n");
    SystemTable->ConOut->OutputString(SystemTable->ConOut, (CHAR16 *)L"Getting memory map and converting it to E820 format...\r\n\n");
    
    __asm__ __volatile__("cli\n hlt");

    return EFI_SUCCESS;
}

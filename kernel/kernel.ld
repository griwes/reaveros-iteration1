ENTRY (start)

SECTIONS
{
    . = 0xc0000000;

    .text ALIGN (0x1000) :
    {
        *(.text)
        *(.gnu.linkonce.t*)
    }

    .rodata ALIGN (0x1000) :
    {
        *(.rodata*)
        *(.gnu.linkonce.r*)
    }

    .data ALIGN (0x1000) :
    {
        *(.data)
        *(.gnu.linkonce.d*)
    }

    .bss :
    {
        sbss = .;
        *(COMMON)
        *(.bss)
        *(.gnu.linkonce.b*)
        ebss = .;
    }

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame) /* c++ exceptions */
    }
}
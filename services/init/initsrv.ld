ENTRY (entry)

SECTIONS
{
    . = 0x100000;

    .text ALIGN (0x1000) :
    {
        *(.text)
        *(.gnu.linkonce.t*)
    }

    .rodata ALIGN (0x1000) :
    {
        *(.rodata*)
        *(.gnu.linkonce.r*)

        . = ALIGN(0x1000);
        start_ctors = .;
        *(SORT(.ctors*))
        end_ctors = .;
    }

    .data ALIGN (0x1000) :
    {
        *(.data)
        *(.gnu.linkonce.d*)

        *(COMMON)
        *(.bss)
        *(.gnu.linkonce.b*)
    }

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
    }
}

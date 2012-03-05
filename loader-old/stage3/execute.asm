global Execute

Execute:
    pop     eax         ; trash caller
    pop     eax         ; memory map address
    pop     ebx         ; memory map size (in number of entries)

    ; kernel needs stack, too... setup it
    ; let it start in some weird place, like from 8MB
    ; why there? well, it's mapped at the moment...
    mov     esp, 0x1000000

    call    0x08:0xc0000000
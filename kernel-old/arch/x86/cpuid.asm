global GetPAEBit

GetPAEBit:
    mov     eax, 0x1
    cpuid

    and     edx, 100000b ; check PAE - CPUID.0x1.EDX.PAE[6]
    cmp     edx, 100000b

    xor     eax, eax

    je      .true

    .true:
        mov     eax, 
        jmp     .end

        mov     eax, 0
    
    .end:
        ret
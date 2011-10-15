;**************************************************************************************************
;
;       ReaverOS
;       stage2.asm
;       second stage bootloader
;
;**************************************************************************************************

bits    16          ; still 16 bit mode
org     0x0500

entry:
    jmp     main

; add some nulls...
times 8 - ($-$$) db 0

;**************************************************************************************************
;
;       Booter loader variables
;
;**************************************************************************************************
size:           dw 0
bootdrive:      dw 0
mmap:           dw 0x7c00

;**************************************************************************************************
;
;       Messages
;
;**************************************************************************************************
msg1:           db "ReaverOS Bootloader v0.1a", 0x0a, 0x0d, 0
msg2:           db "Third stage of bootloader loaded.", 0x0a, 0x0d, 0
msg3:           db "Entering protected mode...", 0x0a, 0x0d, 0

msg4:           db "Protected mode entered.", 0x0a, 0
msg5:           db "Executing third stage bootloader...", 0x0a, 0

;**************************************************************************************************
;
;       Includes
;
;**************************************************************************************************
%include    "stdio.asm"
%include    "gdt.asm"
%include    "a20.asm"
%include    "mmap.asm"

;**************************************************************************************************
;
;       main function
;       setups gdt, a20 and executes C++ Booter
;
;**************************************************************************************************
main:
    pop     word [size]
    pop     word [bootdrive]

    cli
    xor     ax, ax
    mov     ds, ax
    mov     es, ax

    mov     ax, 0x6000
    mov     ss, ax
    mov     sp, 0xffff
    
    sti
    
    mov     si, msg1
    call    print16
    
    mov     si, msg2
    call    print16
    
    mov     si, msg3
    call    print16
    
    cli
    
    call    enable_a20
    call    install_gdt
    
    mov     di, [mmap]
    call    get_memory_map
    
    sti

    ; let's enable protected mode, jump to it and then call C++ BAL
    mov     eax, cr0
    or      al, 1
    mov     cr0, eax
    
    cli
    
    jmp     0x08:pmode
    
;**************************************************************************************************
;
;       Protected Mode entry point
;
;**************************************************************************************************

bits    32

pmode:
    call    clear_screen
    
    mov     ebx, msg4
    call    print
    
    mov     ebx, msg5
    call    print

    mov     ax, 0x10
    mov     ss, ax
    mov     esp, 0x90000
    
    call    get_eip

    xor     edx, edx
    mov     eax, ecx
    ; jump to next 512-aligned address
    ; eip += (0x200 - (i % 0x200))
    ; eax = eip
    mov     ebx, 0x200
    hlt
    div     ebx
    ; edx = eip % 0x200
    sub     ebx, edx
    ; ebx = 0x200 - eip % 0x200
    hlt
    add     ecx, ebx
    
    push    dword [bootdrive]
    
    mov     ebx, 0x200
    mov     eax, [size]
    mul     ebx
    add     eax, 0x500
    push    eax
    
    push    eax

    push    dword [mmap]
    
    ; jump to stage 3: booter
    jmp     ecx                 ; uff, hope it's well computed
        
get_eip:
    mov     ecx, dword [esp]
    ret
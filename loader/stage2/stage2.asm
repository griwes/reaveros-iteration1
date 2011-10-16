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

;**************************************************************************************************
;
;       Booter loader variables
;
;**************************************************************************************************
size:           dw 0
initrdsize:     dw 0
bootersize:     dw 0
bootdrive:      dw 0
booterstart:    dd 0

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
    pop     word [initrdsize]
    pop     word [size]
    pop     word [bootdrive]

    cli
    xor     ax, ax
    mov     ds, ax
    mov     es, ax

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
    
    mov     di, 0x7c00
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
    xor     ax, ax
    mov     ax, word [size]
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
    ; find next 512-aligned address
    ; eip += (0x200 - (i % 0x200))
    ; eax = eip
    mov     ebx, 0x200
    div     ebx
    ; edx = eip % 0x200
    sub     ebx, edx
    ; ebx = 0x200 - eip % 0x200
    add     ecx, ebx
    mov     word [booterstart], cx
    
    xor     eax, eax
    mov     ax, word [bootdrive]
    push    eax
    
    ; Booter starts at 16 MB; Booter's stack starts at 1 MB
    ; 15 MB is even more than it needs...
    push    dword 0x100000

    xor     eax, eax
    mov     ax, word 0x7c00
    push    eax
    
    ; now, move the Booter at 0x1000000 = 16 MB
    ; first: it starts at booterstart
    ; second: compute it's size to know, where initrd will start
    ; third: move Booter at 16 MB and initrd right after it
    ; fourth: pass placement address (right after end of initrd)
    ; fifth: pass initrd address
    ; sixth: execute Booter
    ; looks simple, right?

    xor     edx, edx
    mov     eax, dword [selfsize]
    mov     ebx, 0x200
    div     ebx
    mov     ebx, eax
    cmp     edx, 0
    je      .nope

    inc     ebx
    ; ebx is now selfsize in 0x200 sectors...

    .nope:          
    mov     dword [selfsize], ebx

    mov     eax, dword [size]
    mov     ebx, dword [initrdsize]
    mov     ecx, dword [selfsize]
    sub     eax, ebx
    sub     eax, ecx

    ; eax is now Booter size (or at least should be...)
    mov     dword [bootersize], eax
    hlt

    ; jump to stage 3: booter
    mov     ecx, 0x1000000
    jmp     ecx
        
get_eip:
    mov     ecx, dword [esp]
    ret

selfsize:   dw $ - $$ + 2
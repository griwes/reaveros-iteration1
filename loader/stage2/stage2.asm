;**************************************************************************************************
;
;       ReaverOS
;       stage2.asm
;       second stage bootloader
;
;**************************************************************************************************

bits    16          ; still 16 bit mode
org     0x7e00

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
booterstart:    dw 0
memregcount:    dw 0

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
    cli
    xor     ax, ax
    mov     ds, ax
    mov     es, ax

    sti
    
    pop     word [initrdsize]
    pop     word [size]
    pop     word [bootdrive]

    mov     si, msg1
    call    print16
    
    mov     si, msg2
    call    print16
    
    mov     si, msg3
    call    print16
    
    cli
    
    call    enable_a20
    call    install_gdt
    
    sti
    mov     di, 0x7c00
    call    get_memory_map
    mov     word [memregcount], ax
   
    xor     ax, ax
    xor     bx, bx
    xor     cx, cx
    mov     ax, [size]
    mov     bx, [initrdsize]
    mov     cl, [bootdrive]

    ; let's enable protected mode, jump to it and then call C++ Booter
    mov     edx, cr0
    or      dl, 1
    mov     cr0, edx
    
    cli                 ; it will take long time until we sti against this one

    jmp     0x08:pmode
    
;**************************************************************************************************
;
;       Protected Mode entry point
;
;**************************************************************************************************

bits    32

pmode:
    mov     [size], ax
    mov     [initrdsize], bx
    mov     [bootdrive], cx

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

    ; Booter starts at 16 MB; Booter's "placement stack" starts at 1 MB
    ; 15 MB is even more than it needs...
    push    dword 0x100000

    xor     eax, eax
    mov     ax, word [memregcount]
    push    eax

    xor     eax, eax
    mov     ax, word 0x7c00
    push    eax
    
    ; now, move the Booter at 0x1000000 = 16 MB
    ; first: it starts at booterstart
    ; second: compute it's size to know, where initrd will start
    ; third: move Booter at 16 MB and initrd right after it
    ; fourth: pass initrd address
    ; fifth: execute Booter
    ; looks simple, right?

    xor     eax, eax
    xor     edx, edx
    mov     ax, word [selfsize]
    mov     ebx, 0x200
    div     ebx
    mov     ebx, eax
    cmp     edx, 0
    je      .nope

    inc     ebx
    ; ebx is now selfsize in 0x200 sectors...

    .nope:          
    mov     word [selfsize], bx

    xor     eax, eax
    xor     ebx, ebx
    xor     ecx, ecx

    mov     ax, word [size]
    mov     bx, word [initrdsize]
    mov     cx, word [selfsize]
    sub     eax, ebx
    sub     eax, ecx

    ; eax is now Booter size (yes, it is, checked... maybe not on raw machine, but checked
    ; checked means checked, anyway! well, maybe not in OSDev :P)
    mov     dword [bootersize], eax

    ; push initrd pointer
    mov     ebx, 0x200
    mul     ebx
    add     eax, 0x1000000
    push    eax

    xor     eax, eax
    mov     ax, word [size]
    xor     ecx, ecx
    mov     cx, word [selfsize]

    sub     eax, ecx
    mov     ebx, 0x80           ; 0x200 / 4, as we are moving dwords
    mul     ebx
    mov     ecx, eax

    xor     esi, esi
    mov     si, word [booterstart]

    mov     edi, 0x1000000

    rep     movsd

    ; jump to stage 3: booter
    mov     ecx, 0x1000000
    jmp     ecx
        
get_eip:
    mov     ecx, dword [esp]
    ret

selfsize:   dw $ - $$ + 2
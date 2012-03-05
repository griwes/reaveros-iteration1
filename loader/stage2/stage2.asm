;
; ReaverOS
; loader/stage2/stage2.asm
; Second stage bootloader
;

;
; Reaver Project OS, Rose License
;
; Copyright (C) 2011-2012 Reaver Project Team:
; 1. Michał "Griwes" Dominiak
; 
; This software is provided 'as-is', without any express or implied
; warranty. In no event will the authors be held liable for any damages
; arising from the use of this software.
; 
; Permission is granted to anyone to use this software for any purpose,
; including commercial applications, adn to alter it and redistribute it
; freely, subject to the following restrictions:
; 
; 1. The origin of this software must not be misrepresented; you must not
;    claim that you wrote the original software. If you use this software
;    in a product, an acknowledgment in the product documentation is required.
; 2. Altered source versions must be plainly marked as such, and must not be
;    misrepresented as being the original software.
; 3. This notice may not be removed or altered from any source distribution.
; 
; Michał "Griwes" Dominiak
; 

bits    16
org     0x0

entry:
    jmp     stage2

;
; Variables
;

size:           dw 0
initrdsize:     dw 0
bootersize:     dw 0
bootdrive:      dw 0
booterstart:    dw 0
memregcount:    dw 0

;
; Messages
;

msg1:           db "ReaverOS Bootloader v0.2", 0x0a, 0x0d, 0
msg2:           db "Entering protected mode...", 0x0a, 0x0d, 0
msg3:           db "Protected mode entered.", 0x0a, 0x0d, 0
msg4:           db "Executing third stage...", 0x0a, 0x0d, 0

;
; Includes
;

%include    "stdio.asm"
%include    "gdt.asm"
%include    "a20.asm"
%include    "mmap.asm"

;
; Entry point
;

stage2:
    cli

    mov     ax, 0x07e0
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax

    pop     word [initrdsize]
    pop     word [size]
    pop     word [bootdrive]

    mov     si, msg1
    call    print16

    mov     si, msg2
    call    print16

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

    mov     edx, cr0
    or      dl, 1
    mov     cr0, edx

    cli                         ; long time until we see again, interrupts...

    jmp     0x08:stage3

;
; Stage 3 - protected mode entry
;

bits    32

;
; get_eip()
; Helper function, returning current instruction address
; return - ecx - eip
;

get_eip:
    mov     ecx, dword [esp]
    ret

stage3:
    mov     dx, 0x10
    mov     ds, dx
    mov     es, dx
    mov     fs, dx
    mov     gs, dx
    mov     ss, dx
    mov     esp, 0x90000
    
    mov     [size], ax
    mov     [initrdsize], bx
    mov     [bootdrive], cx

    call    clear_screen

    mov     ebx, msg1
    call    print

    mov     ebx, msg2
    call    print

    mov     ebx, msg3
    call    print

    call    get_eip

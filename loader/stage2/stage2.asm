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
org     0x7e00

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

starting:       dq 0

;
; Messages
;

msg:            db "ReaverOS Bootloader v0.2", 0x0a, 0x0d, 0
vbe:            db "Setting up graphical video mode...", 0x0a, 0x0d, 0

;
; Includes
;

%include    "stdio.asm"
%include    "gdt.asm"
%include    "a20.asm"
%include    "mmap.asm"
%include    "vbe.asm"

;
; Entry point
;

stage2:
    cli

    mov     ax, 0x0
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax

    pop     word [starting + 6]
    pop     word [starting + 4]
    pop     word [starting + 2]
    pop     word [starting]
    pop     word [initrdsize]
    pop     word [size]
    pop     word [bootdrive]

    mov     si, msg
    call    print16

    mov     si, vbe
    call    print16

    call    setup_video_mode

    call    enable_a20
    call    install_gdt

    sti

    mov     di, 0x7c00
    call    get_memory_map
    mov     word [memregcount], ax
    
    mov     edx, cr0
    or      dl, 1
    mov     cr0, edx

    cli                         ; long time until we see again, interrupts...

    jmp     0x08:stage3

;
; Stage 3 - protected mode entry
;

bits    32

stage3:
    ; set registers
    mov     ax, 0x10
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax
    mov     esp, 0x90000

    ; find address of end of stage 2
    mov     ecx, selfsize
    add     ecx, 4

    ; and align it to 0x200 (512)
    add     ecx, 511
    mov     eax, ~511
    and     ecx, eax

    mov     word [booterstart], cx

    push    dword 0x5c00
    
    push    dword video_mode_description

    push    dword [starting + 4]
    push    dword [starting]

    xor     eax, eax
    mov     ax, word [bootdrive]
    push    eax

    push    dword 0x100000

    xor     eax, eax
    mov     ax, word [memregcount]
    push    eax

    xor     eax, eax
    mov     ax, word 0x7c00
    push    eax

    xor     eax, eax
    xor     edx, edx
    mov     ax, word [selfsize]
    mov     ebx, 0x200
    div     ebx
    mov     ebx, eax
    cmp     edx, 0
    je      .nope

    inc     ebx
    
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

    mov     dword [bootersize], eax

    mov     ebx, 0x200
    mul     ebx
    add     eax, 0x800000
    push    eax

    xor     eax, eax
    xor     ecx, ecx
    mov     ax, word [size]
    mov     cx, word [selfsize]

    sub     eax, ecx
    mov     ebx, 0x80
    mul     ebx
    mov     ecx, eax

    xor     esi, esi
    mov     si, word [booterstart]

    mov     edi, 0x800000

    rep     movsd

    jmp     0x800000

selfsize:   dw $ - $$ + 2
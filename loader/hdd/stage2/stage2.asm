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
    jmp     0x0:stage2

times 8 - ($-$$) db 0           ; I don't want to count bytes again...

;
; Variables
;

size:           dw 0            ; 8
bootersize:     dw 0            ; 10
kernelsize:     dw 0            ; 12
initrdsize:     dw 0            ; 14

bootdrive:      dw 0
booterstart:    dd 0x100000
memregcount:    dw 0

starting:       dq 0

;
; Messages
;

msg:            db 0x0a, 0x0d, 0x0a, 0x0d, "ReaverOS Bootloader v0.2", 0x0a, 0x0d, 0
kernel:         db "Loading Booter, kernel and initrd... ", 0
done:           db " done.", 0x0a, 0x0d, 0
vbe:            db "Setting up graphical video mode...", 0x0a, 0x0d, 0
error:          db 0x0a, 0x0d, "Problem with loading... press any key to reboot.", 0x0a, 0x0d, 0
progress:       db ".", 0

;
; Includes
;

%include    "stdio.asm"
%include    "gdt.asm"
%include    "a20.asm"
%include    "mmap.asm"
%include    "vbe.asm"
%include    "int0x13.asm"

;
; Entry point
;

stage2:
    mov     ax, 0x0
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax

    pop     word [starting + 6]
    pop     word [starting + 4]
    pop     word [starting + 2]
    pop     word [starting]
    pop     word [bootdrive]

    mov     si, msg
    call    print16

    mov     si, kernel
    call    print16

    call    enable_a20
    call    install_gdt

    mov     ax, word [size]
    add     word [starting], ax
    inc     word [starting]

    xor     ax, ax
    mov     ax, word [bootersize]
    add     ax, word [kernelsize]
    add     ax, word [initrdsize]

    mov     dl, byte [bootdrive]

    call    read_sectors_high_memory

    mov     si, done
    call    print16

    mov     si, vbe
    call    print16

    call    setup_video_mode

    mov     di, 0x6000
    call    get_memory_map
    mov     word [memregcount], bp

    cli
    mov     al, 0xff            ; mask PIC
    out     0xa1, al
    out     0x21, al
    sti

    nop
    nop
    nop
    nop
    nop

    cli                         ; long time until we see again, interrupts...

    mov     edx, cr0
    or      dl, 1
    mov     cr0, edx

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

    push    dword 0x1000
    push    dword video_mode_description

    xor     eax, eax

    mov     ax, word [initrdsize]
    push    eax
    mov     ax, word [kernelsize]
    push    eax

    mov     ax, word [bootersize]
    mov     bx, 0x200
    mul     bx
    add     eax, dword [booterstart]
    push    eax

    xor     eax, eax
    mov     ax, word [memregcount]
    push    eax

    xor     eax, eax
    mov     ax, word 0x6000
    push    eax

    jmp     0x100000

selfsize:   dw $ - $$ + 2

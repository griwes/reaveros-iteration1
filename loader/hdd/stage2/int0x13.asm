;
; ReaverOS
; loader/stage2/int0x13.asm
; BIOS interrupt number 0x13 routines for 2nd stage bootloader.
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
; including commercial applications, and to alter it and redistribute it
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

packet:
    .size:      db 0x10
    .zero:      db 0
    .number:    dw 0
    .poffset:   dw 0
    .pseg:      dw 0x2000
    .exstart:   dq 0

num:            dw 0

;
; read_sectors_high_memory()
; Reads sectors into memory at 0x800000, doing rmode -> pmode -> rmode jumps.
; ax - number of sectors
;

read_sectors_high_memory:
    push    ax

    mov     edi, 0x800000

    mov     ax, word [starting]
    mov     word [packet.exstart], ax
    mov     ax, word [starting + 2]
    mov     word [packet.exstart + 2], ax
    mov     ax, word [starting + 4]
    mov     word [packet.exstart + 4], ax
    mov     ax, word [starting + 6]
    mov     word [packet.exstart + 6], ax

    pop     ax

    ; 63 sectors per iteration
    .loop:
        cmp     ax, 63
        jle     .le

        mov     word [packet.number], word 63
        sub     ax, 63

        jmp     .counted

    .le:
        mov     word [packet.number], ax
        xor     ax, ax

    .counted:
        mov     dl, byte [bootdrive]
        mov     word [num], ax

        mov     ah, 0x42
        mov     si, packet

        int     0x13

        jc      .error

        mov     si, progress
        call    print16

        mov     edx, cr0
        mov     eax, cr0
        or      al, 1
        mov     cr0, eax

        cli

        push    ds
        push    es

        jmp     0x08:.pmode

bits    32

    .pmode:
        mov     ax, 0x10
        mov     ds, ax
        mov     es, ax
        mov     ss, ax

        mov     ecx, 8064
        mov     esi, 0x20000
        rep     movsd

        jmp     0x18:.pmode16

bits    16

    .pmode16:
        mov     ax, 0x20
        mov     ss, ax

        mov     cr0, edx

        jmp     0x0:.rmode

    .rmode:
        sti

        mov     ax, 0
        mov     ss, ax

        pop     es
        pop     ds

        add     dword [packet.exstart], 768

        mov     ax, word [num]

        cmp     ax, 0
        jne     .loop

        ret

    .error:
        mov     si, error
        call    print16

        xor     ax, ax

        int     0x16
        int     0x19
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

;
; read_sectors_high_memory()
; Reads sectors into memory at 0x800000, doing rmode -> pmode -> rmode jumps.
; ax - number of sectors
; dl - drive number
;

read_sectors_high_memory:
    mov     word [packet.starting], word [starting]
    mov     word [packet.starting + 2], word [starting + 2]
    mov     word [packet.starting + 4], word [starting + 4]
    mov     word [packet.starting + 6], word [starting + 6]

    ; 768 sectors per iteration
    .loop:
        cmp     ax, 768
        jle     .le

        mov     word [packet.number], word 768
        sub     ax, 768

        jmp     .counted

    .le:
        mov     word [packet.number], ax
        xor     ax, ax

    .counted:
        mov     ah, 0x42
        mov     si, packet

        int     0x13

        jc      .error

        mov     edx, cr0
        mov     eax, cr0
        or      al, 1
        mov     cr0, eax

        push    ds
        push    es

        mov     ecx, 192
        mov     esi, 0x20000
        mov     edi, 0x800000
        rep     movsd

        pop     es
        pop     ds

        add     dword [packet.starting], dword 768

        mov     cr0, edx

        cmp     ax, 0
        jne     .loop

        ret

    .error:
        mov     si, error
        call    print16

        xor     ax, ax

        int     0x16
        int     0x19
        
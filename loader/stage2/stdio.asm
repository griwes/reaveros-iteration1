;
; ReaverOS
; loader/stage2/stdio.asm
; Basic screen routines
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

;
; print16()
; si - null ended string
;

print16:
    pushad

    .loop:
        lodsb
        or      al, al
        jz      .done
        mov     ah, 0x0e
        int     0x10
        jmp     .loop
    
    .done:
        popad
        ret

;
; print16_vbe()
; VBE mode version of print16.
; si - null ended string
;

print16_vbe:
    ret


bits    32

curx:   db 0
cury:   db 0

%define     video   0xb8000
%define     cols    80
%define     lines   25
%define     attr    7

;
; clear_screen()
;

clear_screen:
    pusha

    cld

    mov     edi, video
    mov     cx, 2000
    mov     ah, attr
    mov     al, ' '

    rep     stosw

    mov     byte [curx], 0
    mov     byte [cury], 0

    popa
    
    ret

;
; clear_screen_vbe()
; VBE mode version of clear_screen.
; 

clear_screen_vbe:
    ret

;
; putch()
; Prints one char from bl
;

putch:
    pusha

    mov     edi, video

    xor     eax, eax
    mov     ecx, cols * 2
    mov     al, byte [cury]
    mul     ecx
    add     eax, byte [curx]
    add     eax, byte [curx]

    xor     ecx, ecx
    add     edi, eax

    cmp     bl, 0x0a
    je      .row

    mov     dl, bl
    mov     dh, attr

    mov     word [edi], dx
    inc     byte [curx]

    jmp     .done

    .row:
        mov     byte [curx], 0
        inc     byte [cury]

    .done:
        popa
        ret

;
; putch_vbe()
; VBE mode version of putch.
; bl - character
;

putch_vbe:
    ret

;
; print()
; Prints null-terminated string.
; ebx - address of string buffer
;

print:
    pusha
    push    ebx
    pop     edi

    .loop:
        mov     bl, byte [edi]
        cmp     bl, 0
        je      .done

        call    putch
        inc     edi

        jmp     .loop

    .done:
        call    movcur

        popa
        ret

;
; print_vbe()
; VBE mode version of print.
; ebx - address of string buffer
;

print_vbe:
    ret

; 
; put_pixel()
; Helper VBE mode function.
; ax - x
; bx - y
; ecx - pixel color
;

put_pixel:
    ret

;
; movcur()
; Moves hardware cursor.
;

movcur:
    pusha

    xor     eax, eax
    mov     ecx, cols
    mov     al, byte [cury]
    mul     ecx
    add     al, byte [curx]
    mov     ebx, eax

    mov     al, 0x0f
    out     0x03d4, al
    out     0x03d5, bl
    
    mov     al, 0x0e
    out     0x03d4, al
    out     0x03d5, bh

    popa
    ret
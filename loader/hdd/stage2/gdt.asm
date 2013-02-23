;
; ReaverOS
; loader/stage2/gdt.asm
; Basic GDT
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
; install_gdt
; Loads GDT
;

install_gdt:
    pusha

    lgdt    [gdt]

    popa

    ret

;
; GDTs
;

gdt_start:
    ; null - 0x0
    dd 0
    dd 0

    ; code - 0x8
    dw 0xffff
    dw 0
    db 0
    db 10011010b
    db 11001111b
    db 0

    ; data - 0x10
    dw 0xffff
    dw 0
    db 0
    db 10010010b
    db 11001111b
    db 0

    ; 16 bit code - 0x18
    dw 0xffff
    dw 0
    db 0
    db 10011010b
    db 10001111b
    db 0

    ; 16 bit data - 0x20
    dw 0xffff
    dw 0
    db 0
    db 10010010b
    db 10001111b
    db 0

gdt_end:

gdt:
    dw gdt_end - gdt_start - 1
    dd gdt_start
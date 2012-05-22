;
; ReaverOS
; kernel/entry.asm
; Kernel's entry point.
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

bits    64

global  entry
extern  kernel_main

entry:
    mov     rax, qword 0xFFFFFFFF80000000
    add     rax, byte 32
    jmp     rax

times 32 - ($-$$) db 0

highmemory:
    push    rbp
    mov     rbp, rsp

    mov     rdi, qword [rsp + 8]
    mov     rsi, qword [rsp + 16]
    mov     edx, dword [rsp + 24]
    mov     rcx, qword [rsp + 28]
    mov     r8, qword [rsp + 36]

    mov     rax, rsi
    ; stupid loop, TODO: sanity checks
    .loop:          
        cmp     dword [rax + 16], 0xffff
        je     .after

        add     rax, 24
        jmp     .loop

    .after:
        mov     rsp, qword [rax + 8]
        mov     rax, qword 0xFFFFFFFF80000000
        add     rsp, rax

    mov     rbp, rsp

    jmp     kernel_main
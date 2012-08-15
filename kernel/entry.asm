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
    mov     rax, qword highmemory
    jmp     rax

highmemory:
    mov     rbp, rsp

    xchg    bx, bx

    pop     rdi
    pop     rsi
    mov     edx, dword [rsp]
    add     rsp, 4
    pop     rcx
    pop     r8
    pop     r9
    
    mov     rax, rsi
    mov     r10d, edx

    ; stupid loop, TODO: sanity checks
    ; the loop below moves kernel stack at the end of currently (and later) mapped area
    ; (I was little clueless, when I looked at this few days after writing it)
    .loop:      
        cmp     dword [rax + 16], 0xffff
        je      .after

        add     rax, 24
        dec     r10d
        
        cmp     r10d, 0
        je      .fail
        
        jmp     .loop

    .after:
        mov     rsp, qword [rax + 8]
        mov     rax, qword 0xFFFFFFFF80000000
        add     rsp, rax
        
    mov     rbp, rsp

    jmp     kernel_main
    
    .fail:
        hlt
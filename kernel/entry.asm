;
; Reaver Project OS, Rose License
;
; Copyright (C) 2011-2013 Reaver Project Team:
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

extern  kernel_main

extern  start_ctors
extern  end_ctors

global  entry

entry:
    push    rax
    push    rbx
    push    rcx

    mov     rbx, qword start_ctors
    mov     rcx, qword end_ctors
    jmp     .loop

    .call:
        xchg    bx, bx

        call    [rbx]
        add     rbx, 8

    .loop:
        cmp     rbx, rcx
        jge     .call

    pop     rcx
    pop     rbx
    pop     rax

    call    kernel_main

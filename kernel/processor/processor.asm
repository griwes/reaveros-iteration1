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

global  get_cr3
global  reload_cr3
global  initial_id

get_cr3:
    mov     rax, cr3
    ret

reload_cr3:
    push    rax
    mov     rax, cr3
    mov     cr3, rax
    pop     rax

    ret

initial_id:
    push    rbx
    push    rcx
    push    rdx

    mov     rax, 0

    mov     eax, 1
    cpuid

    bt      ecx, 21
    jc      .x2apic

    mov     eax, ebx
    shr     eax, 24

    pop     rdx
    pop     rcx
    pop     rbx

    ret

    .x2apic:
        mov     eax, 0x0b
        cpuid

        mov     eax, edx

        pop     rdx
        pop     rcx
        pop     rbx

        ret

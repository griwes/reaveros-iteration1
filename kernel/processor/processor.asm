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
global  initialize
global  _load_idt

extern  idtr

get_cr3:
    mov     rax, cr3
    ret

reload_cr3:
    push    rax
    mov     rax, cr3
    mov     cr3, rax
    pop     rax
    
    ret

initialize:
    push    rax
    mov     rax, qword gdt
    lgdt    [rax]

    mov     rax, qword .ret
    jmp     0x8:rax

    .ret:
    mov     ax, 0x10
    mov     fs, ax
    mov     gs, ax
    
    pop     rax
    
    ret
    
_load_idt:
    mov     rax, qword idtr
    lidt    [rax]
    
    ret

gdt_start:
    ; 0x0 null:
    dq 0

    ; 0x8 code 64 bit: 
    dw 0
    dw 0
    db 0
    db 10011000b
    db 00100000b
    db 0

    ; 0x10 data:
    dw 0
    dw 0
    db 0
    db 10010000b
    db 0
    db 0
    
gdt_start:
    dq 0 ; null
    dq 0 ; code segment 64bit kernel
    dq 0 ; data segment 64bit kernel
    dq 0 ; code segment 64bit user 
    dq 0 ; code segment 64bit user
    dq 0 ; TSS

gdt_end:

gdt:
    dw gdt_end - gdt_start - 1
    dq gdt_start

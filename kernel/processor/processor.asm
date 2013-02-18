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
global  _load_gdt
global  _load_idt_from
global  _gdt_start
global  _lock_bit
global  _unlock_bit
global  _load_gdt_from
global  __lock
global  __unlock

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

_load_gdt:
    mov     rax, qword gdt
    lgdt    [rax]

    mov     rax, qword .ret

    push    0x8
    push    rax
    
    retf

    .ret:
    mov     ax, 0x10
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax
    
    mov     ax, 0x2B
    ltr     ax
    
    ret
    
_load_idt_from:
    lidt    [rdi]
    
    sti
    
    ret

_load_gdt_from:
    lgdt    [rdi]
    
    mov     rax, qword .ret
    
    push    0x8
    push    rax
    
    retf
    
    .ret:
    mov     ax, 0x10
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax
    
    mov     ax, 0x2B
    ltr     ax
    
    ret
    
_gdt_start:
    dq 0 ; null
    dq 0 ; code segment 64bit kernel - 0x8
    dq 0 ; data segment 64bit kernel - 0x10
    dq 0 ; code segment 64bit user - 0x18
    dq 0 ; code segment 64bit user - 0x20
    dq 0 ; TSS - 0x28
    dq 0

gdt_end:

gdt:
    dw gdt_end - _gdt_start - 1
    dq _gdt_start

_lock_bit:
    cli

    lock    bts     qword [rdi], rsi
    
    jc      .fail

;    call    disable_scheduler

    sti
    
    ret
    
    .fail:
        sti
    
        clc
        pause
        
        jmp     _lock_bit

_unlock_bit:
    mov     cx, si

    mov     rax, 1
    shl     rax, cl
    not     rax
    
    lock    and     qword [rdi], rax
    
;    call    enable_scheduler
    
    ret

__lock:
    cli

    mov     al, 1
    cmpxchg byte [rdi], al
    
    cmp     al, 1
    
    je      .retry
    
;    call    disable_scheduler

    sti
    
    ret
    
    .retry:
        pause
        
        jmp     __lock
    
__unlock:
    mov     byte [rdi], 0
    
;    call    enable_scheduler
    
    ret

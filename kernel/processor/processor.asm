;
; Reaver Project OS, Rose License
;
; Copyright © 2011-2013 Michał "Griwes" Dominiak
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
global  set_cr3
global  reload_cr3
global  initial_id
global  load_gdt
global  load_idt
global  common_interrupt_stub
global  x2apic_capable
global  isr_context_return

extern  _common_interrupt_handler

get_cr3:
    mov     rax, cr3
    ret

set_cr3:
    mov     cr3, rdi
    ret

reload_cr3:
    push    rax
    mov     rax, cr3
    mov     cr3, rax
    pop     rax

    ret

x2apic_capable:
    push    rbx
    push    rcx
    push    rdx

    mov     rax, 0

    mov     eax, 1
    cpuid

    bt      ecx, 21
    jc      .x2apic

    mov     rax, 0

    pop     rdx
    pop     rcx
    pop     rbx

    ret

    .x2apic:
        mov     rax, 1

    pop     rdx
    pop     rcx
    pop     rbx

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

load_gdt:
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

load_idt:
    lidt    [rdi]

    ret

common_interrupt_stub:
    push    rax
    push    rbx
    push    rcx
    push    rdx
    push    rsi
    push    rdi
    push    rbp
    push    r8
    push    r9
    push    r10
    push    r11
    push    r12
    push    r13
    push    r14
    push    r15

    call    _common_interrupt_handler

isr_context_return:
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     r11
    pop     r10
    pop     r9
    pop     r8
    pop     rbp
    pop     rdi
    pop     rsi
    pop     rdx
    pop     rcx
    pop     rbx
    pop     rax

    add     rsp, 16

    iretq

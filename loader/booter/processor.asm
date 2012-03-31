;
; ReaverOS
; loader/booter/processor.asm
; Assembly routines for x86-64 processors.
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

bits    32

;
; extern "C"
; {
;     uint32 _cpu_check_long_mode();
;     void _enable_pae_paging();
;     void _enable_msr_longmode();
;     void _enable_paging(uint32);
; }
;

global _cpu_check_long_mode
global _enable_pae_paging
global _enable_msr_longmode
global _enable_paging

_cpu_check_long_mode:
    mov     eax, 0x80000000
    cpuid
    cmp     eax, 0x80000001
    jb      .no

    mov     eax, 0x80000001
    cpuid
    test    edx, 1 << 29
    jz      .no

    mov     eax, 1

    ret

    .no:
        xor     eax, eax

        ret


_enable_pae_paging:
    mov     eax, cr4
    or      eax, 1 << 5
    mov     cr4, eax

    ret


_enable_msr_longmode:
    mov     ecx, 0xC0000080
    rdmsr
    or      eax, 1 << 8
    wrmsr

    ret

_enable_paging:
    pop     ebp
    pop     eax

    mov     cr3, eax

    mov     eax, cr0
    or      eax, 1 << 31
    mov     cr0, eax

    push    dword 0
    push    ebp

    ret
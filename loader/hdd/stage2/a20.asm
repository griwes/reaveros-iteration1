;
; ReaverOS
; loader/stage2/a20.asm
; A20 gate routine
;

;
; Reaver Project OS, Rose License
;
; Copyright © 2011-2012 Michał "Griwes" Dominiak
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

enable_a20:
    ; try BIOS A20 enable function
    mov     ax, 0x2401
    int     0x15

    ; check whether it was successful
    cmp     ah, 0
    je      .end

    ; it wasn't, use keyboard controller method
    cli

    call    a20wait
    mov     al, 0xad
    out     0x64, al

    call    a20wait
    mov     al, 0xd0
    out     0x64, al

    call    a20wait2
    mov     al, 0x64
    push    eax

    call    a20wait
    mov     al, 0xd1
    out     0x64, al

    call    a20wait
    pop     eax
    or      al, 2
    out     0x60, al

    call    a20wait
    mov     al, 0xae
    out     0x64, al

    call    a20wait

    sti

    .end:
        ret

;
; Helper functions
;

a20wait:
    in      al, 0x64
    test    al, 2
    jnz     a20wait
    ret

a20wait2:
    in      al, 0x64
    test    al, 1
    jz      a20wait2
    ret
global  trampoline_start
global  trampoline_end

bits    16

trampoline_start:
;    mov     byte [cs:8], 1
    jmp     $

times   8 - ($ - $$)    db 0

flag:   dq 0
    
trampoline_end:
db      0

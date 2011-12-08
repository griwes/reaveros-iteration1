;**************************************************************************************************
;
;       ReaverOS
;       stdio.asm
;       basic I/O routines for stage 2 and 3
;
;**************************************************************************************************
bits    16

;**************************************************************************************************
;
;       print16()
;       si - null-ended string to be printed
;
;**************************************************************************************************
print16:
    pusha
    .loop:
        lodsb
        or      al, al
        jz      .done
        mov     ah, 0eh
        int     10h
        jmp     .loop
    .done:
        popa
        ret
        
;**************************************************************************************************
;
;		32bit stuff
;
;**************************************************************************************************
bits    32

curx    db 0
cury    db 0

%define     video   0xb8000
%define     cols    80
%define     lines   25
%define     attr    7           
                    ; white on black

;**************************************************************************************************
;
;       clear_screen()
;
;**************************************************************************************************
clear_screen:
    pusha
    cld
    mov     edi, video
    mov     cx, 2000
    mov     ah, attr
    mov     al, ' '
    rep     stosw

    mov     byte [curx], 0
    mov     byte [cury], 0
    popa
    ret

;**************************************************************************************************
;
;       putch()
;       prints char from bl
;
;**************************************************************************************************
putch:
    pusha
    mov     edi, video

    ; y * 2 * columns +...
    xor     eax, eax
    mov     ecx, cols * 2
    mov     al, byte [cury]
    mul     ecx
    push    eax

    ; ...+ 2 * curx is current offset in vidmem
    mov     al, byte [curx]
    mov     cl, 2
    mul     cl
    pop     ecx
    add     eax, ecx

    xor     ecx, ecx
    add     edi, eax

    cmp     bl, 0x0a
    je      .row

    mov     dl, bl
    mov     dh, attr
    
    mov     word [edi], dx
    inc     byte [curx]
    
    jmp     .done
    
    .row:
        mov     byte [curx], 0
        inc     byte [cury]
    
    .done:
        popa
        ret
        
;**************************************************************************************************
;
;       print
;       prints null-ended string at current position
;       ebx - address of buffer to print
;
;**************************************************************************************************
print:
    pusha
    push    ebx
    pop     edi

    .loop:
        mov     bl, byte [edi]
        cmp     bl, 0
        je      .done

        call    putch
        inc     edi

        jmp     .loop

    .done:
        call    movcur

        popa
        ret

;**************************************************************************************************
;
;       movcur()
;       updates cursor position
;
;**************************************************************************************************
movcur:
    pusha
    
    xor     eax, eax
    mov     ecx, cols
    mov     al, byte [cury]
    mul     ecx
    add     al, byte [curx]
    mov     ebx, eax

    mov     al, 0x0f
    mov     dx, 0x03d4
    out     dx, al

    mov     al, bl
    mov     dx, 0x03d5
    out     dx, al
    
    mov     al, 0x0e
    mov     dx, 0x03d4
    out     dx, al
    
    mov     al, bh
    mov     dx, 0x03d5
    out     dx, al
    
    popa
    ret
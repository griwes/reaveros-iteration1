;**********************************************************************************************************************
;
;       ReaverOS
;       bootloader.asm
;       First stage bootloader
;
;**********************************************************************************************************************

bits    16
org     0x0

entry:
    jmp     main

times 8 - ($-$$) db 0                                                               ; - 8

;**********************************************************************************************************************
;
;       Reaver header
;
;**********************************************************************************************************************
magic:          dd 0xFEA7EFF5   ; ReaveRFS, I'm sure you can find it here           9, 10, 11, 12
sectorsize:     dw 512          ; size of single sector                             13, 14
nsectors:       dw 2880         ; number of sectors                                 15, 16
tracksectors:   dw 18           ; number of sectors per track                       17, 18
cylinderheads:  dw 2            ; number of heads per cylinder                      19, 20
reserved:       dw 0            ; number of reserved sectors                        21, 22
stage2size:     dw 0            ; size of stage 2, filled by reaver floppy creator  23, 24

; so, to be remembered, 21-22 and 23-24 bytes must be updated by reaver floppy creator
; stage2size is in sectors, not kBs or anything else

starting:       dw 0

packet:
    size        db 10h
    zero        db 0
    number      dw 0
    poffset     dw 0
    pseg        dw 0x0050
    exstart     dq 0

;***********************************************************************************************************************
;
;       Bootloader's variables
;
;***********************************************************************************************************************
msg             db "Loading...", 0x0a, 0x0d, 0
progress        db ".", 0
failmsg         db 0x0a, 0x0d, "Error", 0

bootdrive       db 0

sector          db 0
head            db 0
track           db 0

;***********************************************************************************************************************
;
;       print()
;       si - null terminated string
;
;***********************************************************************************************************************
print:
    .start:
        lodsb                   ; load next character
        or      al, al          ; is it null?
        jz      .done           ; then we finished
        mov     ah, 0eh         ; print
        int     10h             ; ..., print!
        jmp     .start          ; and next character
    .done:
        ret                     ; we are done

;***********************************************************************************************************************
;
;       absolute_address()
;       ax - address to be converted
;
;***********************************************************************************************************************
absolute_address:
    xor     dx, dx
    div     word [tracksectors]
    inc     dl
    mov     byte [sector], dl
    xor     dx, dx
    div     word [cylinderheads]
    mov     byte [head], dl
    mov     byte [track], al
    ret

;***********************************************************************************************************************
;
;       read_sectors()
;       cx - number of sectors
;       ax - starting sector
;       es:bx - buffer to read to
;
;***********************************************************************************************************************
read_sectors:
    .begin:
        mov     di, 0x005
    .loop:
        push    ax
        push    bx
        push    cx
        
        call    absolute_address        ; convert address
        
        mov     ah, 0x02                ; read sector
        mov     al, 0x01                ; read ONE sector
        
        mov     ch, byte [track]
        mov     cl, byte [sector]
        
        mov     dh, byte [head]
        mov     dl, byte [bootdrive]
        
        int     0x13
        
        jnc     .end                    ; test read error
        
        xor     ax, ax
        int     0x13                    ; reset disc
        
        dec     di
        
        pop     cx
        pop     bx
        pop     ax
        
        jnz     .loop

        mov     si, failmsg
        call    print
        
        cli
        hlt
    
    .end:
        mov     si, progress
        call    print
        
        pop     cx
        pop     bx
        pop     ax
        
        add     bx, word [sectorsize]
        inc     ax
        
        loop    .begin
        
        ret

;***********************************************************************************************************************
;
;       check_extended()
;       Checks, if bootdisk support disk extensions
;       ax: 0 if not present
;
;***********************************************************************************************************************
check_extended:
    mov     ax, 1

    cmp     dl, 0
    je      .no
    cmp     dl, 1
    je      .no
    
    mov     ah, 0x41
    mov     bx, 0x55aa
    
    int     0x13
    
    jnc     .return
    
    .no:
        xor     ax, ax
        ret
        
    .return:
        mov     ax, 1
        ret

;***********************************************************************************************************************
;
;       read_sectors_extended
;       Highly awesome, requires hdd (!)
;       starting sector is exstarting
;       cx - number of sectors
;       i wish number of sectors will fit in cx...
;
;***********************************************************************************************************************
read_sectors_extended:
    mov     ah, 0x42
    mov     dl, [bootdrive]
    mov     si, packet
    
    mov     word [number], cx
    
    int     0x13
    
    jc      .error
    
    ret
    
    .error:
        mov     si, failmsg
        call    print
        cli
        hlt

;***********************************************************************************************************************
;
;       Entry point
;
;***********************************************************************************************************************
main:
    cli                             ; we should not be interrupted now, no matter what

    mov     ax, 0x07c0              ; our address
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax

    xor     ax, ax                  ; create stack
    mov     ss, ax
    mov     sp, 0x7c00
    sti                             ; restore interrupts

    xor     ax, ax
    mov     word [bootdrive], ax
    mov     [bootdrive], dl         ; save boot drive number

    mov     si, msg
    call    print                   ; print our pretty welcome message
    
    call    check_extended
    cmp     ax, 0

    jne     extended

    mov     ax, 0x07e0
    mov     es, ax
    mov     bx, 0x0000
    
    mov     cx, word [reserved]
    dec     cx
    mov     ax, word [starting]
    inc     ax
    call    read_sectors
    
    jmp     s2jump

extended:
    mov     cx, word [reserved]
    dec     cx
    call    read_sectors_extended

s2jump:
    mov     cx, word [bootdrive]
    push    word [bootdrive]
    mov     cx, word [reserved]
    dec     cx
    push    cx

    ; compute initrd size
    mov     ax, [reserved]
    sub     ax, [stage2size]
    dec     ax
    push    word ax

    push    word 0x07e0
    push    word 0x0000

    mov     si, progress
    call    print

    retf
       
;***********************************************************************************************************************
;
;       Boot signature
;
;***********************************************************************************************************************
times   510 - ($-$$) db 0               ; fill up to 510th byte

dw      0xaa55                          ; boot signature
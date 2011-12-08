;**************************************************************************************************
;
;       ReaverOS
;       mmap.asm
;       Memory map getter
;
;**************************************************************************************************

bits    16

;**************************************************************************************************
;
;       get_memory_map
;       gets memory map into buffer starting at ES:DI
;       return: eax = number of regions or 0 if not supported
;
;**************************************************************************************************
get_memory_map:
        xor     bp, bp

        mov     eax, 0xe820
        xor     ebx, ebx
        mov     ecx, 24
        mov     edx, 0x534d4150
        
        mov     [es:di + 20], dword 1
        
        int     0x15
        
        jc      .fail
        
        mov     edx, 0x534d4150
        cmp     eax, edx
        jne     .fail
        
        cmp     ebx, 0
        je      .fail
        
        inc     bp
        
    .loop:
        mov     eax, 0xe820
        mov     ecx, 24
        mov     edx, 0x534d4150
        add     di, 24
        
        mov     [es:di + 20], dword 1
        
        int     0x15
        
        jc      .end
        
        cmp     ebx, 0
        je      .end
        
        inc     bp
        jmp     .loop
        
    .end:
        xor     eax, eax
        mov     ax, bp
        xor     bp, bp
        clc
        ret
        
    .fail:
        xor     eax, eax
        ret
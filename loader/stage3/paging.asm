global LoadCR3
global EnablePaging

LoadCR3:
    pop     eax
    mov     cr3, eax
    ret

EnablePaging:
    mov     eax, cr0
    or      eax, 0x80000000
    mov     cr0, eax
    ret
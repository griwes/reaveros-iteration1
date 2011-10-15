bits    32

outb:
    pop     byte ax
    pop     word dx
    out     dx, ax
    ret
    
inb:
    pop     word dx
    in      ax, dx
    ret
    
global  outb
global  inb

global  booter_main
extern  booter_main

start:
    call booter_main
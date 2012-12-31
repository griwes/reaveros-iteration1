bits    64

extern  kernel_main

global  entry

entry:
    xchg    bx, bx

    call    kernel_main
    
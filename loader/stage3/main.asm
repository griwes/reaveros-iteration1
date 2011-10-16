bits    32

global  booter_main
extern  booter_main

global start

start:
    call booter_main            ; never returns

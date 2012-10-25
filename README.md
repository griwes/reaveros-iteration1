#Rose, Reaver Project OS

ReaverOS is part of larger - and yet to become actively developed and gather a team - project,
from which it has taken its name - Reaver Project, meant to provide full computer environment.
More about it on [its website](http://reaver-project.org/).

ReaverOS is an attempt to build modern operating system, aimed at x86-64 architecture with
SMP and NUMA in mind. It is also aiming to be as much legacy free system as possible.

It's not POSIX; to be honest, it will have nothing to do with POSIX other than few basic command
line tool names. Rose will provide unique, object-oriented programming environment, as opposed
to file-oriented environment of UNIX family.

ReaverOS is going to be hybrid kernel. Everything that runs either per-(logical) core or per-NUMA
domain will be kernel space; everything else will run as process, to easy synchronization, as
each NUMA domain will be operating its own instance of the kernel.

##What's already here?

 1. First stage HDD bootloader
 2. Second stage bootloader, featuring:
   * Setting VBE video mode
   * Making sense out of BIOS memory map
   
##What's going to be here?

 1. Booter - last stage bootloader, featuring:
   * Booting NUMA Domain Bootstrap Processors - NBP
   * Starting kernel instances on each NUMA Domain
 2. Kernel, featuring:
   * Preemptive, NUMA and SMP aware multitasking
   * NUMA aware virtual memory manager
   * Virtual Input/Output LAyer - VIOLA, allowing both filesystem and device access use the same
     syscalls, without confusing people with mounting devices into VFS
   * Network stack, featuring native support for some kind of Reliable UDP, built upon UDP to
     allow easy use of it on other systems - just write a layer over UDP socket
   * GUI, centered around graphical console-like shell, allowing seamless integration of typed
     terminal and graphical filesystem manager, featuring easy to use GUI toolkit

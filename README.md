#Rose, Reaver Project OS

ReaverOS is part of larger - and yet to become actively developed and gather a team - project,
from which it has taken its name - Reaver Project, meant to provide full computer environment.
More about it on [its website](http://reaver-project.org/).

ReaverOS is an attempt to build modern operating system, aimed at x86-64 architecture with
SMP in mind. It is also aiming to be as much legacy free system as possible, while implementing
modern, µkernel design.

It's not POSIX; to be honest, it will have nothing to do with POSIX other than few basic command
line tool names. Rose will provide unique, object-oriented programming environment, as opposed
to file-oriented environment of UNIX family.

##What's already here?

 1. Bootloader, with HDD and VESA mode support
 2. Kernel:
   * Basic initialization of outputting and physical memory management
   * Basic ACPI table support (no AML here)
   * Basic SMP support - booting APs and IPIs
   
##What is being worked on? - short roadmap for current development

 1. Kernel:
   * Basic SMP support - some special IPIs
   * SMP aware physical memory manager
   * SMP aware scheduler
 2. Services:
   * SMP aware virtual memory manager
   
##What are the features that are planned in distant future?

 1. Virtual Input/Output LAyer - VIOLA, allowing both filesystem and device access use the same
    syscalls, without confusing people with mounting devices into VFS
 2. Network stack, featuring native support for some kind of Reliable UDP, built upon UDP to
    allow easy use of it on other systems - just write a layer over UDP socket
 3. GUI, centered around graphical console-like shell, allowing seamless integration of typed
    terminal and graphical filesystem manager, featuring easy to use GUI toolkit

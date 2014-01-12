# Rose, Reaver Project OS

ReaverOS is part of larger - and yet to become actively developed and gather a team - project, from which it has taken
its name - Reaver Project, meant to provide full computer environment. More about it on
[its website](http://reaver-project.org/).

ReaverOS is an attempt to build modern operating system, aimed at x86-64 architecture with SMP in mind. It is also aiming
to be as legacy free as possible, while implementing modern, µkernel design.

## Current features

  * kernel timer API using HPET (or PIT, if HPET is not available) for high precision timer and local APIC timer for
preemption timer
  * AP booting

## Build instructions

To build ReaverOS, you will need following tools installed on your system:

  * POSIX shell (d'oh)
  * make (I am personally using GNU make, since I am on Linux; not sure if I am using some specific GNU things, hopefully
not)
  * wget
  * tar
  * git
  * clang++, libc++ for building the kernel, binutils and other tools (I have **no plans** of supporting GCC at any point
in the future)
  * [ReaverLib](http://github.com/griwes/ReaverLib)

If any of those required tools doesn't cooperate during build, feel free to contact me (Griwes @ #reaver on Freenode,
@Guriwesu on Twitter), but be prepared to get an answer saying "upgrade your tools". I usually Clang's HEAD every month
or so, and using every bleeding edge features I feel I need.

To clone and build current revision of Rose, type the following commands in your shell:

    git clone git://github.com/griwes/ReaverOS.git
    cd ReaverOS
    make prepare
    make tools
    make hdd

**It is recommended to use a release, not current HEAD, to build the OS, if you want to just try and see it. To get a list
of releases, along with links to download their source code tarballs, visit https://github.com/griwes/ReaverOS/releases
(see warning below before choosing release to build). You should still invoke both `make prepare` and `make hdd` before
using following instructions to run the OS.**

To run ReaverOS, you'll need one of the following:

  * Bochs - to invoke Bochs, use plain `make` after or instead `make hdd`. `make prepare` will create 1GiB image and bochsrc
file that will be used with Bochs by default; if you wish to provide own image (maybe smaller? who knows), consult the bochs
manual.
  * QEMU - to invoke QEMU, type `make q`, it will start the emulator.
  * VirtualBox - to use VirtualBox, type `make vbox`. This will create a VirtualBox disk image (`vbox.vdi`) in `builds/`
directory; use that image as boot image of virtual machine created by you in VirtualBox.

If you really want to run ReaverOS on real hardware (this is possibly a bad idea; in the current state of the OS, it shouldn't
be capable of doing any harm, but it is extremely untested), use `builds/a.img` as a raw image of an USB stick and boot it.

**Note**: as of current version, ReaverOS is only capable of having a working scheduler on multiple core processors. It will
be eliminated one day, but today is not such a day.

## Debug build

If you encounter any bug and want to report it, please attach entire (or as much as you can grab, in case of VirtualBox
or real hardware) output of kernel built using these commands:

    make clean                  # clean the non-debug build

followed by one of the following:

    make CFL=-DROSEDEBUG        # create a debug build and run it in Bochs, OR:
    make q CFL-DROSEDEBUG       # to run it in QEMU, OR:
    make vbox CFL=-DROSEDEBUG   # to create a VirtualBox image with the OS, or:
    make hdd CFL=-DROSEDEBUG    # to just create builds/a.img to be written on USB stick

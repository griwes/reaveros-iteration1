#!/bin/bash

cd ./loader/stage1/
nasm stage1.asm -o ../../../reaverfs/bootfloppy/stage1.img
cd ../stage2/
nasm stage2.asm -o ../../../reaverfs/bootfloppy/stage2.img

cd ../stage3/
make clean
make
mv ./booter.img ../../../reaverfs/bootfloppy/kernel.img

cd ../../../reaverfs/bootfloppy/
./bootfloppy a.img stage1.img stage2.img kernel.img
qemu -fda a.img -s -S

#!/bin/bash

rm ../reaverfs/bootfloppy/a.img
rm ../reaverfs/bootfloppy/stage1.img
rm ../reaverfs/bootfloppy/stage2.img
rm ../reaverfs/bootfloppy/kernel.img

cd ./loader/stage1/
nasm stage1.asm -o ../../../reaverfs/bootfloppy/stage1.img
cd ../stage2/
nasm stage2.asm -o ../../../reaverfs/bootfloppy/stage2.img

cd ../stage3/
make clean
make
mv ./booter.img ../../../reaverfs/bootfloppy/stage3.img

cd ../../../reaverfs/initramdisk/
./initrd initrd.img listfile
mv ./initrd.img ../bootfloppy/

cd ../bootfloppy/
./bootfloppy a.img stage1.img stage2.img stage3.img initrd.img
qemu -fda a.img -s -S

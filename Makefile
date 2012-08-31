all: hdd bochs

q: hdd qemu

u: uefi qemu-uefi

bochs:
	cd builds && bochs -q

qemu:
	cd builds && qemu-system-x86_64 -hda hdd.img -monitor stdio -no-kvm -m 2048

uefi-qemu:
	cd builds/efi && qemu-system-x86_64 -L . -bios OVMF.fd -m 2048 -cpu kvm64 -hda efidisk.hdd -enable-kvm

hdd:
	cd loader/hdd; \
	yasm stage1.asm -o ../../builds/stage1.img
	cd loader/hdd/stage2; \
	yasm stage2.asm -o ../../../builds/stage2.img
	cd loader/booter; \
	make; \
	mv builds/booter.img ../../builds/
#	cd kernel; \
#	make; \
#	mv builds/kernel.img ../builds/
	cd builds; \
	./mkrfloppy a.img stage1.img stage2.img booter.img kernel.img stage1.img; \
	dd if=a.img of=hdd.img conv=notrunc

uefi:
	cd loader/uefi; \
	make
	cd builds/efi; \
	cp $(UDKPATH)/MyWorkSpace/Build/MdeModule/RELEASE_GCC46/X64/roseuefi.efi ./EFI/BOOT/BOOTX64.EFI; \
	sudo losetup --offset 1048576 --sizelimit 66060288 /dev/loop0 efidisk.hdd; \
	sudo mount /dev/loop0 ./mount; \
	sudo mv ./EFI/BOOT/BOOTX64.EFI ./mount/EFI/BOOT/BOOTX64.EFI
	sleep 3s
	make unmount

unmount:
	cd builds/efi && sudo umount ./mount && sudo losetup -d /dev/loop0

clean:
	cd loader/booter; \
	make clean
	cd kernel; \
	make clean

uefi-clean: clean
	cd loader/uefi; \
	make clean

prepare:
	mkdir -p builds
	mkdir -p loader/booter/builds
	mkdir -p kernel/builds
	cd builds && dd if=/dev/zero of=a.img bs=1G count=1
	cd builds && g++ -std=c++0x ../utils/mkrfloppy/main.cpp -o mkrfloppy

tools:
	cd utils && ./build-tools.sh

build-bochs:
	cd utils && ./build-bochs.sh
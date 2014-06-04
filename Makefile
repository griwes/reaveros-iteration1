all: hdd bochs

q: hdd qemu

u: uefi qemu-uefi

bochs:
	cd builds && bochs -q

qemu:
	qemu-system-x86_64 -smp 4,cores=4 -monitor stdio -m 2048 -parallel file:/dev/stdout -hda builds/hdd.img -enable-kvm \
		-cpu qemu64,+sse3,+sse4.1,+sse4.2,+x2apic
uefi-qemu:
	cd builds/efi && qemu-system-x86_64 -L . -bios OVMF.fd -m 2048 -cpu kvm64 -hda efidisk.hdd -enable-kvm

vbox: hdd
	@rm builds/vbox.vdi
	VBoxManage convertfromraw builds/hdd.img builds/vbox.vdi --format vdi
	virtualbox

hdd:
	cd loader/hdd; \
	yasm stage1.asm -o ../../builds/stage1.img
	cd loader/hdd/stage2; \
	yasm stage2.asm -o ../../../builds/stage2.img
	cd loader/booter; \
	make; \
	mv builds/booter.img ../../builds/
	cd kernel; \
	make; \
	mv builds/kernel.img ../builds/
	cd library/rose; \
	make; \
	mv librose.a ../
	cd services/init; \
	make; \
	mv init.srv ../
	cd services; \
	raf -c init.srv -o ../builds/initrd.img
	cd builds; \
	./mkrfloppy a.img stage1.img stage2.img booter.img kernel.img initrd.img; \
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
	cp doc/bochsrc builds/
	cd builds && clang++ -std=c++11 ../utils/mkrfloppy/main.cpp -o mkrfloppy

tools:
	cd utils && ./build-tools.sh

build-bochs:
	cd utils && ./build-bochs.sh

analyze:
	cd loader/booter; \
	make CFL="--analyze -Xanalyzer -analyzer-output=text"
	cd kernel; \
	make CFL="--analyze -Xanalyzer -analyzer-output=text"

debug:
	cd loader/booter; \
	make CFL="-DROSEDEBUG" -j12
	cd kernel; \
	make CFL="-DROSEDEBUG" -j12

bd:
	make debug
	make bochs

qd:
	make debug
	make qemu

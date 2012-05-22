all: clean chdd bochs

q: clean chdd qemu

bochs:
	cd builds && bochs -q

qemu:
	cd builds && qemu-system-x86_64 -hda hdd.img -monitor stdio -no-kvm -m 2048

chdd:
	colormake hdd

hdd:
	cd loader/hdd; \
	yasm stage1.asm -o ../../builds/stage1.img
	cd loader/hdd/stage2; \
	yasm stage2.asm -o ../../../builds/stage2.img
	cd loader/booter; \
	colormake; \
	mv builds/booter.img ../../builds/
	cd kernel; \
	colormake; \
	mv builds/kernel.img ../builds/
	cd builds; \
	./mkrfloppy a.img stage1.img stage2.img booter.img kernel.img stage3.img; \
	dd if=a.img of=hdd.img conv=notrunc

clean:
	cd loader/booter; \
	colormake clean
	cd kernel; \
	colormake clean
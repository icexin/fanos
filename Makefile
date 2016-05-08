.PHONY:all kernel begin mount umount
all: kernel

kernel:
	scons -Q
	ld -T script/linker.ld -okernel.bin libfanos.a

mount:
	mkdir -p ramfs osimg
	sudo mount  -o loop,offset=32256 images/hd.img osimg
	sudo cp images/ramfs.img osimg/ramfs.img
	sudo mount -o loop osimg/ramfs.img ramfs

umount:
	sudo cp osimg/ramfs.img images/ramfs.img
	sudo umount -f ramfs
	sudo umount -f osimg

begin:kernel
	sudo cp kernel.bin osimg/kernel.bin && sync
	bochs -q -f script/bochsrc
	#qemu-system-i386 -m 32 -hda images/hd.img -serial stdio

debug:kernel
	sudo cp kernel.bin osimg/kernel.bin &&sync
	#bochs_dbg -q   -f script/bochsrc_dbg &
	qemu-system-i386 images/hd.img -serial stdio -s -S -machine accel=tcg

clean:
	rm -f kernel.bin
	scons -c

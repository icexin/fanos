CC=gcc
ASM=nasm
CFLAGS:=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -g -Iinclude -Wall -DDEBUG

.PHONY:all kernel begin mount umount
all: kernel

kernel:kernel.bin

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

vmlinux26.bin:kernel.bin
	objcopy -O binary $^ $@
	cp $@ /var/www/weblinux/ -f

dump:
	objdump -D kernel.bin | less

libfanos.a:
	scons -Q

kernel.bin:libfanos.a
	ld -T script/linker.ld -o$@ libfanos.a

clean:
	rm -f kernel.bin
	scons -c

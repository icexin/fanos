CC=gcc
ASM=nasm
CFLAGS:=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -g -Iinclude -Wall -DDEBUG

.PHONY:all kernel begin mount umount init kern lib fs apps
all: kernel apps

kernel:kernel.bin

mount:
	sudo mount  -o loop,offset=32256 images/hd.img osimg
	sudo cp images/ramfs.img osimg/ramfs.img
	sudo mount -o loop osimg/ramfs.img ramfs

umount:
	sudo cp osimg/ramfs.img images/ramfs.img
	sudo umount -f ramfs
	sudo umount -f osimg

begin:kernel apps
	sudo cp kernel.bin osimg/kernel.bin && sync
	#bochs -q -f script/bochsrc
	qemu-system-i386 -m 32 -hda images/hd.img -serial stdio
debug:kernel apps
	sudo cp kernel.bin osimg/kernel.bin &&sync
	#bochs_dbg -q   -f script/bochsrc_dbg &
	qemu-system-i386 images/hd.img -serial stdio -s -S -machine accel=tcg

local:kernel apps
	sudo cp kernel.bin osimg/kernel.bin &&sync
	bochs -q   -f script/bochsrc

vmlinux26.bin:kernel.bin
	objcopy -O binary $^ $@
	cp $@ /var/www/weblinux/ -f

dump:
	objdump -D kernel.bin | less
init:
	$(MAKE) -C init
kern:
	$(MAKE) -C kernel
lib:
	$(MAKE) -C lib
fs:
	$(MAKE) -C fs
apps:
	$(MAKE) -C apps

kernel.bin:init kern lib fs
	ld -T script/linker.ld -o$@ init/init.o kernel/kernel_all.o lib/lib.o fs/fs.o

clean:umount
	rm -f kernel.bin
	$(MAKE) -C init clean
	$(MAKE) -C kernel clean
	$(MAKE) -C lib clean
	$(MAKE) -C fs clean
	$(MAKE) -C apps clean

CC=gcc
ASM=nasm
CFLAGS:=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -g -Iinclude

.PHONY:all kernel run mount umount
all: kernel

kernel:kernel.bin

mount:
	if ! grep 'osimg' /etc/mtab ;then \
		sudo mount -t ext2 -o loop,offset=32256 images/hd.img osimg \
	;fi 
umount:
	sudo umount osimg


run:mount kernel
	sudo cp kernel.bin osimg/kernel.bin 
	sudo strip --strip-debug osimg/kernel.bin && sync
	bochs -q   -f script/bochsrc
debug:mount kernel
	sudo cp kernel.bin osimg/kernel.bin 
	sudo strip --strip-debug osimg/kernel.bin && sync
	bochs -q   -f script/bochsrc_dbg&
	gdb
kernel.bin:kernel/kernel.o kernel/head.o kernel/global.o kernel/asm.o lib/string.o
	ld -T script/linker.ld $^ -o$@
clean:umount
	rm -f kernel.bin
	rm -f kernel/*.o
	rm -f lib/*.o

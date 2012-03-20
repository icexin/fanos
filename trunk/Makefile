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
	if grep 'osimg' /etc/mtab; then \
		sudo umount osimg \
	;fi


run:mount kernel
	sudo cp kernel.bin osimg/kernel.bin 
	sudo strip --strip-debug osimg/kernel.bin && sync
	bochs -q   -f script/bochsrc
debug:mount kernel
	sudo cp kernel.bin osimg/kernel.bin 
	sudo strip --strip-debug osimg/kernel.bin && sync
	bochs -q   -f script/bochsrc_dbg 2> /dev/null &
	gdb
local:mount kernel
	sudo cp kernel.bin osimg/kernel.bin 
	sudo strip --strip-debug osimg/kernel.bin && sync
	bochs_local -q   -f script/bochsrc
kernel/global.o:include/global.h include/kernel.h



kernel.bin:init/main.o kernel/system.o kernel/head.o kernel/asm.o lib/string.o kernel/tty.o kernel/keyboard.o kernel/sys_call.o kernel/schedule.o kernel/trap.o
	ld -T script/linker.ld $^ -o$@
clean:umount
	rm -f kernel.bin
	rm -f kernel/*.o
	rm -f lib/*.o

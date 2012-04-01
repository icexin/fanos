CC=gcc
ASM=nasm
CFLAGS:=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -g -Iinclude

.PHONY:all kernel run mount umount clean_dep
all: kernel

kernel:.depend  kernel.bin

include .depend


mount:
	if ! grep 'osimg' /etc/mtab ;then \
		mkdir -p /tmp/tmpfs; \
		sudo mount tmpfs /tmp/tmpfs -t tmpfs;\
		cp images/hd.img /tmp/tmpfs/hd.img;\
		sudo mount  -o loop,offset=32256 /tmp/tmpfs/hd.img osimg \
	;fi 
umount:
	if grep 'osimg' /etc/mtab; then \
		sudo umount osimg \
		sudo umount /tmp/tmpfs \
	;fi


begin:mount kernel
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

vmlinux26.bin:kernel.bin
	objcopy -O binary $^ $@
	cp $@ /var/www/weblinux/ -f

C_SRC:=init/main.c  kernel/keyboard.c kernel/schedule.c kernel/trap.c \
	kernel/hd.c kernel/kernel.c kernel/mem.c kernel/serial.c \
	kernel/system.c kernel/tty.c lib/string.c kernel/read_write.c

ASM_SRC:=kernel/head.S kernel/asm.S kernel/sys_call.S 

OBJ:=$(ASM_SRC:%.S=%.o) $(C_SRC:%.c=%.o) 

.depend:$(C_SRC)
	gcc $(CFLAGS) -M $^ > $@

kernel.bin:$(OBJ)
	ld -T script/linker.ld $^ -o$@
clean:umount
	rm -f kernel.bin
	rm -f kernel/*.o
	rm -f lib/*.o
	rm -f .depend

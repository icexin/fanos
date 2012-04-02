CC=gcc
ASM=nasm
CFLAGS:=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -g -Iinclude

.PHONY:all kernel begin mount umount init kern lib
all: kernel

kernel:kernel.bin


mount:
	if ! grep 'osimg' /etc/mtab ;then \
		mkdir -p /tmp/tmpfs&& \
		sudo mount tmpfs /tmp/tmpfs -t tmpfs&&\
		cp images/hd.img /tmp/tmpfs/hd.img&&\
		sudo mount  -o loop,offset=32256 /tmp/tmpfs/hd.img osimg \
	;fi 
umount:
	if grep 'osimg' /etc/mtab; then \
		sudo umount osimg ; \
		sudo umount /tmp/tmpfs;\
	fi


begin:mount kernel
	sudo cp kernel.bin osimg/kernel.bin 
	sudo strip --strip-debug osimg/kernel.bin &&sync
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
init:
	$(MAKE) -C init
kern:
	$(MAKE) -C kernel
lib:
	$(MAKE) -C lib

kernel.bin:init kern lib
	ld -T script/linker.ld -o$@ init/init.o kernel/kernel_all.o lib/lib.o
clean:umount
	rm -f kernel.bin
	$(MAKE) -C init clean
	$(MAKE) -C kernel clean
	$(MAKE) -C lib clean

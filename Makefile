CC=gcc
ASM=nasm
CFLAGS:=-nostdlib -nostdinc -fno-builtin -fno-stack-protector -g -Iinclude

.PHONY:all kernel begin mount serial umount init kern lib fs
all: kernel

kernel:kernel.bin


mount:
	if ! grep 'osimg' /etc/mtab ;then \
		mkdir -p /tmp/tmpfs&& \
		sudo mount tmpfs /tmp/tmpfs -t tmpfs&&\
		cp images/hd.img /tmp/tmpfs/hd.img&&\
		sudo mount  -o loop,offset=32256 /tmp/tmpfs/hd.img osimg&&\
		sudo cp images/ramfs.img osimg/ramfs.img&&\
		sudo mount -o loop osimg/ramfs.img ramfs \
	;fi 
umount:
	if grep 'osimg' /etc/mtab; then \
		cp -f /tmp/tmpfs/hd.img images/hd.img; \
		cp -f osimg/ramfs.img images/ramfs.img; \
		sudo umount ramfs; \
		sudo umount osimg ; \
		sudo umount /tmp/tmpfs;\
	fi

serial:
	if [ ! -e "serial" ]; then mkfifo serial; fi

begin:mount serial kernel
	sudo cp kernel.bin osimg/kernel.bin && sync
	bochs -q   -f script/bochsrc
debug:mount serial kernel
	sudo cp kernel.bin osimg/kernel.bin &&sync
	bochs -q   -f script/bochsrc_dbg 2> /dev/null &
	gdb
local:mount serial kernel
	sudo cp kernel.bin osimg/kernel.bin &&sync
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
fs:
	$(MAKE) -C fs

kernel.bin:init kern lib fs
	ld -T script/linker.ld -o$@ init/init.o kernel/kernel_all.o lib/lib.o fs/fs.o

clean:umount
	rm -f kernel.bin
	$(MAKE) -C init clean
	$(MAKE) -C kernel clean
	$(MAKE) -C lib clean
	$(MAKE) -C fs clean

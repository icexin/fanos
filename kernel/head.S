.global loader
.extern main

.set ALIGN, 1<<0
.set MEMINFO, 1<<1
.set FLAGS, ALIGN|MEMINFO
.set MAGIC, 0x1BADB002
.set CHECKSUM, -(MAGIC + FLAGS)

.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

.set STACKSIZE, 0x4000
.comm stack, STACKSIZE, 32

loader:
	mov $(stack + STACKSIZE), %esp
	push %eax
	push %ebx
	cli
	call main
	sti

hang:
	hlt
	jmp hang

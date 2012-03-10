.global keyboard
.global timer
.global enable_int
.global disable_int
.global move_to_user

.extern handle
.extern putchar
.extern taska
.extern taskb
.extern choice
.text
keyboard:
	call handle
	mov $0x20, %al
	out %al, $0x20
	iret
timer:
	push %ds
	push %eax
	push %ebx

	mov $0x20, %al
	out %al, $0x20

	mov $0x10, %ax
	mov %ax, %ds
	mov choice, %eax
	lea choice, %ebx
	test %eax, %eax
	jz 1f
	#none zero jump to taskb
	movl $0, (%ebx)
	ljmp $0x30, $0
	jmp 2f

1:
	movl $1, (%ebx)
	ljmp $0x28, $0
2:
	pop %ebx
	pop %eax
	pop %ds

	iret
enable_int:
	sti
	ret
disable_int:
	cli
	ret
move_to_user:
	pushfl
	andl $0xFFFFbFFF, (%esp)
	popfl
	movl $0x28, %eax
	ltr %ax
	movl $0x18, %eax
	lldt %ax
	sti
	mov $0x17, %ax
	mov %ax, %ds
	mov %esp, %eax
	pushl $0x17
	pushl %eax
	pushfl
	pushl $0x0F
	pushl $taska
	iret
	

.global keyboard
.global timer
.global enable_int
.global disable_int
.global move_to_user

.extern handle
.extern do_timer
.text
keyboard:
	call handle
	mov $0x20, %al
	out %al, $0x20
	iret
timer:
	push %ds
	push %es
	push %fs
	push %eax
	push %ebx
	push %ecx
	push %edx

	mov $0x20, %al
	out %al, $0x20
	call do_timer

	pop %edx
	pop %ecx
	pop %ebx
	pop %eax
	pop %fs
	pop %es
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
	movl $0x18, %eax
	ltr %ax
	movl $0x20, %eax
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
	

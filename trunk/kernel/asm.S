.global keyboard
.global timer
.global enable_int
.global disable_int
.global move_to_user

.extern handle
.extern handle_timer
.extern putchar
.extern taska
.extern taskb
.text
keyboard:
	call handle
	mov $0x20, %al
	out %al, $0x20
	iret
timer:
	mov $0x20, %al
	out %al, $0x20
	call handle_timer
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
	mov $0x17, %ax
	mov %ax, %ds
	sti
	pushl $0x17
	pushl %esp
	pushfl
	pushl $0x0F
	pushl $taska
	ret
	

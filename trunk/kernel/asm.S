.global keyboard
.global timer
.global enable_int
.global disable_int
.global move_to_user
.global taska
.global taskb

.extern handle
.extern handle_timer
.extern putchar
.extern taska
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
	iret
	
taska:
	mov $0xb8000, %ebx
	mov $0x17, %ax
	mov %ax, %gs
	mov $'a', %al
L1:	movb %al, %gs:(%ebx)
	jmp L1


taskb:
	mov $0xb8000, %ebx
	mov $0x17, %ax
	mov %ax, %gs
	mov $'b', %al
L2:	movb %al, %gs:(%ebx)
	jmp L2

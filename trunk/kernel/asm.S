.global keyboard
.global timer
.global enable_int
.global disable_int
.extern handle
.extern handle_timer
.extern putchar
.text
keyboard:
	call handle
	mov $0x20, %al
	out %al, $0x20
	iret
timer:
	call handle_timer
	mov $0x20, %al
	out %al, $0x20
	iret
	iret
enable_int:
	sti
	ret
disable_int:
	cli
	ret

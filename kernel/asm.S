.global keyboard
.global enable_int
.global disable_int
.extern handle
.text
keyboard:
	call handle
	mov $0x20, %al
	out %al, $0x20
	iret
enable_int:
	sti
	ret
disable_int:
	cli
	ret

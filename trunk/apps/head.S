.global _start
.extern main
.extern exit

.code32
.text
_start:
	call main
	push %eax
	call exit


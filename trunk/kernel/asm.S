.global keyboard
.global timer
.global enable_int
.global disable_int
.global move_to_user
.global divide_error, debug, nmi, break_point, over_flow, bounds
.global invalid_op, coprocessor, double_fault, co_overflow, invalid_tss
.global invalid_seg, stack_error, general_protect, page_error, co_error
.global reverse

.extern handle
.extern do_timer
.text
keyboard:

	push %eax
	push %ebx
	push %ecx
	push %edx
	push %esi
	push %edi

	push %ds
	mov $0x10, %eax
	mov %ax, %ds

	call handle
	mov $0x20, %al
	out %al, $0x20
	pop %ds
	pop %edi
	pop %esi
	pop %edx
	pop %ecx
	pop %ebx
	pop %eax
	iret
timer:
	push %ds
	push %es
	push %fs
	push %eax
	push %ebx
	push %ecx
	push %edx
	
	mov $0x10, %eax
	mov %ax, %ds

	incl ticks #系统嘀嗒

	mov $0x20, %al
	out %al, $0x20

#取cpl
	mov 32(%esp), %eax
	andl $3, %eax
	push %eax
	call do_timer
	add $4, %esp

	pop %edx
	pop %ecx
	pop %ebx
	pop %eax
	pop %fs
	pop %es
	pop %ds
	iret

move_to_user:
	pushfl
	andl $0xFFFFbFFF, (%esp)
	popfl
	movl $0x18, %eax
	ltr %ax
	movl $0x20, %eax
	lldt %ax
	mov $0x17, %ax
	mov %ax, %ds
	sti
	pushl $0x17
	pushl $0x202000 
	pushfl
	pushl $0x0F
	pushl $taska
	iret
.equ no_error_code, 0xFFFFFFFF
divide_error:
	push no_error_code
	push $0
	call exception_handler
debug:
	push no_error_code
	push $1
	call exception_handler
nmi:
	push no_error_code
	push $2
	call exception_handler
break_point:
	push no_error_code
	push $3
	call exception_handler
over_flow:
	push no_error_code
	push $4
	call exception_handler
bounds:
	push no_error_code
	push $5
	call exception_handler
invalid_op:
	push no_error_code
	push $6
	call exception_handler
coprocessor:
	push no_error_code
	push $7
	call exception_handler
double_fault:
	push $8
	call exception_handler
co_overflow:
	push no_error_code
	push $9
	call exception_handler
invalid_tss:
	push $10
	xchg %bx, %bx
	call exception_handler
invalid_seg:
	push $11
	call exception_handler
stack_error:
	push $12
	call exception_handler
general_protect:
	xchg %bx, %bx
	push $13
	call exception_handler
page_error:
	push $14
	call exception_handler
co_error:
	push no_error_code
	push $16
	call exception_handler

reverse:
	call do_reverse
	iret






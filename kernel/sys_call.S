.extern sys_call_table

.global sys_call #系统调用中断

nr_system_calls = 2
bad_sys_call:
	mov $-1, %eax
	iret
sys_call:
	cmpl nr_system_calls - 1, %eax #超出调用号范围退出
	ja bad_sys_call

	push %ds
	push %es
	push %fs
	push %esi
	push %edi
	push %ebp


#依次把参数压栈,%ebx,%ecx,%edx依次为第1，2，3个参数
	pushl %edx
	pushl %ecx
	pushl %ebx

#ds,es指向内核数据段
	mov $0x10, %edx
	mov %dx, %ds
	mov %dx, %es
#fs指向局部数据段
	mov $0x17, %edx
	mov %dx, %fs

	call *sys_call_table(,%eax,4)
	add $12, %esp #恢复参数栈

	pop %ebp
	pop %edi
	pop %esi
	pop %fs
	pop %es
	pop %ds
	iret



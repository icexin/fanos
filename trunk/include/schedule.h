#ifndef _SHCHEDULE_H
#define _SHCHEDULE_H
#include <type.h>
#include <system.h>

struct tss_t
{
	u32 backlink;
	u32 esp0;
	u32 ss0;
	u32 esp1;
	u32 ss1;
	u32 esp2;
	u32 ss2;
	u32 cr3;
	u32 eip;
	u32 eflags;
	u32 eax;
	u32 ecx;
	u32 edx;
	u32 ebx;
	u32 esp;
	u32 ebp;
	u32 esi;
	u32 edi;
	u32 es;
	u32 cs;
	u32 ss;
	u32 ds;
	u32 fs;
	u32 gs;
	u32 ldt;
	u16 trap;
	u16 iobase;
}__attribute__((packed));

struct task_t
{
	struct tss_t tss;
	struct desc ldt[3];	
	unsigned long start_addr;
	int pid;
	int ticks;
	int priority;
};


typedef int (*fn_ptr)();
#define NR_TASK 10
#define TASK_SIZE (2<<20)
#define FIRST_TSS 3
#define FIRST_LDT (FIRST_TSS + 1)
#define _TSS(n) (((unsigned long)n) * 0x10 + (FIRST_TSS << 3)) //第n个tss的选择符
#define _LDT(n) (((unsigned long)n) * 0x10 + (FIRST_LDT << 3)) //第n个ldt的选择符

#define switch_to(n) { \
struct{long a,b;}__tmp; \
__asm__("cmpl %%ecx, current_task\n\t" \
	"je 1f\n\t" \
	"movl %%ecx, current_task\n\t" \
	"movw %%dx, %1\n\t" \
	"movb $'0', (0xB8000)\n\t" \
	"xchg %%bx, %%bx\n" \
	"ljmp %0\n\t" \
	"1:\n\t" \
	::"m"(*&__tmp.a),"m"(*&__tmp.b), \
	"d"(_TSS(n)),"c"(task[n])); \
}

#define move_to_user() do{ \
	__asm__( \
	"pushfl\n\t" \
	"andl $0xFFFFbFFF, (%%esp)\n\t" \
	"popfl\n\t" \
	"movl $0x18, %%eax\n\t" \
	"ltr %%ax\n\t" \
	"movl $0x20, %%eax\n\t" \
	"lldt %%ax\n\t" \
	"mov %%esp, %%eax\n\t" \
	"sti \n\t" \
	"pushl $0x17\n\t" \
	"pushl %%eax \n\t" \
	"pushfl\n\t" \
	"pushl $0x0F\n\t" \
	"pushl $1f\n\t" \
	"#2:xchg %%bx, %%bx\n\t" \
	"iret\n\t" \
	"1:mov $0x17, %%ax\n\t" \
	"mov %%ax, %%ds\n\t" \
	"#2:jmp 2b\n\t" \
	:::"eax");\
}while(0)

extern struct task_t *task[];
extern struct task_t *current_task;

void init_sched();
int  create_task(int task_no, fn_ptr task_fn);
void schedule();
#endif

#ifndef _SHCHEDULE_H
#define _SHCHEDULE_H
#include <type.h>

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
	int taskno;
};


typedef int (*fn_ptr)();
#define TASK_SIZE 10
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
	"ljmp %0\n\t" \
	"1:\n\t" \
	::"m"(*&__tmp.a),"m"(*&__tmp.b), \
	"d"(_TSS(n)),"c"(task_struct[n])); \
}


void init_sched();
int  create_task(int task_no, fn_ptr task_fn);
void schedule();
#endif

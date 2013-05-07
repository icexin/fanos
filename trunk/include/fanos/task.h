#ifndef _TASK_H
#define _TASK_H
#include <fanos/system.h>
#include <stdint.h>

#define RUN  0
#define WAIT 1<<0
#define HANG 1<<1

#define NR_TASK 10
#define TASK_SIZE (2<<20)

// 每个任务在gdt里面都有一个tss和ldt,刚好占16(0x10)个字节.
// 第一个gdt为null, 第二个和第三个分别为全局代码段和数据段
// 第三个为task0的tss段,第四个为task0的ldt(task0需要ldt吗)
#define FIRST_TSS 3
#define FIRST_LDT (FIRST_TSS + 1)
#define _TSS(n) (((unsigned long)n) * 0x10 + (FIRST_TSS * 8)) //第n个tss的选择符距离gdt起始地址的偏移(按字节算)
#define _LDT(n) (((unsigned long)n) * 0x10 + (FIRST_LDT * 8)) //第n个ldt的选择符距离gdt起始地址的偏移(按字节算)
#define GDT_TSS(n) (gdt+FIRST_TSS+(n)*2)
#define GDT_LDT(n) (gdt+FIRST_LDT+(n)*2)


// 临时变量b保存有新任务的tss, ljmp到tss上就完成了任务转换
#define switch_to(n) { \
char __tmp[6]; \
__asm__("cmpl %%ecx, current_task\n\t" \
	"je 1f\n\t" \
	"movl %%ecx, current_task\n\t" \
	"movw %%dx, %1\n\t" \
	"ljmp *%0\n\t" \
	"1:\n\t" \
	::"m"(__tmp[0]),"m"(__tmp[4]), \
	"d"(_TSS(n)),"c"(task[n])); \
}

// task0的手动进入用户模式
// 先关闭nt标志
// 再开启task0中的中断,这个时候还是关闭的
#define move_to_user() do{ \
	__asm__( \
	"pushfl\n\t" \
	"andl $0xFFFFBFFF, (%%esp)\n\t" \
	"popfl\n\t" \
	"movl $0x18, %%eax\n\t" \
	"ltr %%ax\n\t" \
	"movl $0x20, %%eax\n\t" \
	"lldt %%ax\n\t" \
	"mov %%esp, %%eax\n\t" \
	"pushl $0x17\n\t" \
	"pushl %%eax \n\t" \
	"pushfl\n\t" \
	"orl $0x0000200, (%%esp)\n\t" \
	"pushl $0x0F\n\t" \
	"pushl $1f\n\t" \
	"sti\n\t" \
	"iret\n\t" \
	"1:mov $0x17, %%ax\n\t" \
	"mov %%ax, %%ds\n\t" \
	"mov %%ax, %%es\n\t" \
	"mov %%ax, %%fs\n\t" \
	"mov %%ax, %%gs\n\t" \
	:::"eax");\
}while(0)



typedef uint32_t pid_t;

struct tss_t
{
	uint32_t backlink;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;
	uint16_t trap;
	uint16_t iobase;
}__attribute__((packed));

struct task_t
{
	struct tss_t tss;
	struct desc ldt[3];	
	void* start_addr;
	pid_t pid;
	pid_t ppid;
	uint32_t priority;
	uint32_t status;
	int exit_status;
	uint8_t istask;
	size_t ticks;
	size_t timeout;
	size_t start_ticks;
	size_t cputime;
};

extern struct task_t *task[];
extern struct task_t *current_task;

struct task_t* create_task(int (*task_fn)());
struct task_t* create_task0();
int find_empty_process();

void* get_user_addr(struct task_t* t, void* addr);

// 交出控制权, 进入wait状态
void sleep_on(struct task_t*);
void wake_up(struct task_t*);

#endif

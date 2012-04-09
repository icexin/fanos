#include <schedule.h>
#include <mem.h>
#include <string.h>
#include <kernel.h>

extern int taska();
extern int taskb();
extern int taskc();
extern struct desc gdt[];

struct task_t *current_task;
struct task_t *task_struct[TASK_SIZE];
unsigned long ticks; /*系统嘀嗒数*/

static void create_task0()
{
	struct task_t * task0 = get_free_page();
}
static void create_task_tss(struct task_t *t, fn_ptr task_fn)
{
	struct tss_t *tss = &(t->tss);

	tss->esp0 = (int)(t) + PAGE_SIZE;
	tss->ss0 = 0x10;
	tss->eflags = 0x200;
	tss->esp = get_free_page() + PAGE_SIZE;
	tss->es = 0x17;
	tss->cs = 0x0F;
	tss->ss = 0x17;
	tss->ds = 0x17;
	tss->fs = 0x17;
	tss->gs = 0x17;
	tss->eip = (u32)task_fn;
	tss->ldt = _LDT(t->pid);
	
	/*加入gdt*/
	create_gdt_desc(gdt + FIRST_TSS + t->pid * 2, (u32)(tss), sizeof(struct task_t) - 1, DA_386TSS);
}

static void create_task_ldt(struct task_t *t)
{
	struct desc *d = t->ldt;
	create_ldt_desc(d, 0, 0, 0);
	create_ldt_desc(d + 1, 0, 0xFFFFF, DA_DPL3 | DA_32 | DA_CR | DA_LIMIT_4K );
	create_ldt_desc(d + 2, 0, 0xFFFFF, DA_DPL3 | DA_32 | DA_DRW | DA_LIMIT_4K );
	
	/*加入gdt*/
	create_gdt_desc(gdt + FIRST_LDT + t->pid * 2, (u32)d, sizeof(struct desc) * 3 - 1, DA_LDT);
	
}

void init_sched()
{
	create_task(0, taska);
	create_task(1, taskb);
	create_task(2, taskc);
	current_task = task_struct[0];
}


int create_task(int pid, fn_ptr task)
{
	assert(pid < TASK_SIZE);
	if(pid > TASK_SIZE)
		return 0;
	struct task_t *t = get_free_page();
	t->pid = pid;
	task_struct[pid] = t;
	create_task_tss(t, task);
	create_task_ldt(t);

	return 1;
}

void do_timer(int cpl)
{
	if(!cpl)return;
	schedule();
}


void schedule()
{
	int next;
	char* const video = (char *const)0xB8000;
	next = (current_task->pid + 1) % 3;
//	printf("current=%x pid=%d\n", current_task, current_task->taskno);
//	printf("next=%x pid=%d\n", task_struct[next], next);
	switch_to(next);
}

#include <schedule.h>
#include <system.h>

extern int taska();
extern int taskb();
extern int taskc();
extern struct desc gdt[];

int current_task;
struct task_t task_struct[TASK_SIZE];
struct desc ldt[TASK_SIZE * 3];

static void create_task_tss(int task_no, fn_ptr task_fn)
{
	struct tss_t *tss = &(task_struct[task_no].tss);

	tss->esp0 = (u32)(task_struct[task_no].stack + 1024 * 4 - 1);
	tss->ss0 = 0x10;
	tss->eflags = 0x200;
	tss->esp = 0x200000 + 0x100000 * (task_no + 1); //1M-2M为内核内存，从2M开始为任务内存，每个任务1M空间
	tss->es = 0x17;
	tss->cs = 0x0F;
	tss->ss = 0x17;
	tss->ds = 0x17;
	tss->fs = 0x17;
	tss->gs = 0x17;
	tss->eip = (u32)task_fn;
	tss->ldt = _LDT(task_no);
	
	/*加入gdt*/
	create_gdt_desc(gdt + FIRST_TSS + task_no * 2, (u32)(task_struct + task_no), sizeof(struct task_t) - 1, DA_386TSS);
}

static void create_task_ldt(int task_no)
{
	struct desc *d = &ldt[task_no * 3];
	create_ldt_desc(d, 0, 0, 0);
	create_ldt_desc(d + 1, 0, 0xFFFFF, DA_DPL3 | DA_32 | DA_CR | DA_LIMIT_4K );
	create_ldt_desc(d + 2, 0, 0xFFFFF, DA_DPL3 | DA_32 | DA_DRW | DA_LIMIT_4K );
	
	/*加入gdt*/
	create_gdt_desc(gdt + FIRST_LDT + task_no * 2, (u32)d, sizeof(struct desc) * 3 - 1, DA_LDT);
	
}

void init_sched()
{
	current_task = 0;
	create_task(0, taska);
	create_task(1, taskb);
	create_task(2, taskc);
}


int create_task(int task_no, fn_ptr task)
{
	if(task_no > TASK_SIZE)
		return 0;
	create_task_tss(task_no, task);
	create_task_ldt(task_no);
	return 1;
}

void do_timer()
{
	schedule();
}

void schedule()
{
	int next;
	next = (current_task + 1) % 3;
	current_task = next;
	move_to(next);
}

#include <schedule.h>
#include <mem.h>
#include <string.h> 
#include <kernel.h> 

int cnt = '0'; 
struct task_t *current_task; 
struct task_t *task[NR_TASK]; 
unsigned long ticks; /*系统嘀嗒数*/ 
static void create_task_tss(struct task_t *t, fn_ptr task_fn);
static void create_task_ldt(struct task_t *t);

static void create_task0()
{
	struct task_t *task0 = get_free_page();
	struct tss_t  *tss = &(task0->tss);

	memset(task0, 0, sizeof(*task0));
	/* add to task struct */
	task[0] = task0;

	task0->ppid = 0;
	task0->pid = 0;
	task0->start_addr = 0;
	task0->ticks = task0->priority = 10;
	task0->status = RUN;
	task0->istask = 0;
	create_task_ldt(task0);

	task0->tss.esp0 = (int)task0 + PAGE_SIZE;
	tss->ss0 = 0x10;
	tss->cs = 0x0F;
	tss->ds = 0x17;
	tss->ss = 0x17;
	tss->fs = 0x17;
	tss->eflags = 0x200;
	tss->ldt = _LDT(0);

	create_gdt_desc(gdt + FIRST_TSS, (u32)(tss), sizeof(struct task_t) - 1, DA_386TSS);
}

static void create_task_tss(struct task_t *t, fn_ptr task_fn)
{
	struct tss_t *tss = &(t->tss);

	tss->esp0 = (int)(t) + PAGE_SIZE;
	tss->ss0 = 0x10;
	tss->eflags = 0x200;
	tss->esp = (int)(get_free_page() + PAGE_SIZE);
	tss->es = 0x10;
	tss->cs = 0x08;
	tss->ss = 0x10;
	tss->ds = 0x10;
	tss->fs = 0x17;
	tss->gs = 0x10;
	tss->eip = (u32)task_fn;
	tss->ldt = _LDT(t->pid);
	
	/*加入gdt*/
	create_gdt_desc(gdt + FIRST_TSS + t->pid * 2, (u32)(tss), sizeof(struct task_t) - 1, DA_386TSS);
}

static void create_task_ldt(struct task_t *t)
{
	struct desc *d = t->ldt;
	create_ldt_desc(d, 0, 0, 0);
	create_ldt_desc(d + 1, t->start_addr, 0x1FF, DA_DPL3 | DA_32 | DA_CR | DA_LIMIT_4K );
	create_ldt_desc(d + 2, t->start_addr, 0x1FF, DA_DPL3 | DA_32 | DA_DRW | DA_LIMIT_4K );
	
	/*加入gdt*/
	create_gdt_desc(gdt + FIRST_LDT + t->pid * 2, (u32)d, sizeof(struct desc) * 3 - 1, DA_LDT);
	
}

void init_sched()
{
	create_task0();
	current_task = task[0];
}


int create_task(fn_ptr taskptr)
{
	int pid = find_empty_process();

	assert(pid != -1);
	struct task_t *t = get_free_page();
	memset(t, 0, sizeof(*t));
	t->pid = pid;
	t->ticks = t->priority = 10;
	t->istask = 1;
	t->start_addr = 0;
	task[pid] = t;
	create_task_tss(t, taskptr);
	create_task_ldt(t);
	return 1;
}

void do_timer(int cpl)
{
		
	if(cpl == 0 && current_task->istask == 0){
//		printk("Timer:process %d in kernel\n", current_task->pid);
		return;
	}
	schedule();
}


void schedule()
{
	struct task_t *t, *ready = task[0];
	int larger_ticks = 0;
	int i;
	
	cli();
	for(i=0; i<NR_TASK; i++){
		t = task[i];
		if(!t || t == current_task)continue;
		if(t->ticks > larger_ticks && t->status == RUN){
			larger_ticks = t->ticks;
			ready = t;
		}
	}

	if(larger_ticks <= 0){
		for(i = 0; i<NR_TASK; i++){
			t = task[i];
			if(!t || t == current_task)continue;
			t->ticks = t->priority;
			if(t->priority > larger_ticks && t->status == RUN){
				larger_ticks = t->priority;
				ready = t;
			}
		}
	}
	
	/* if no process is running run process 0 */
	ready->ticks--;
/*	__asm__(
	"mov %0, %%es\n\t"
	"mov %1, %%gs\n\t"
	::"r"(current_task->pid),"r"(ready->pid));
*/
	assert(ready != current_task);

//	printk("next process :%d\n", ready->pid);
//	printk("current process :%d\n", current_task->pid);
	switch_to(ready->pid);
}

#include <fanos/task.h>
#include <fanos/mem.h>
#include <fanos/time.h>
#include <fanos/schedule.h>
#include <fanos/kernel.h>
#include <string.h>

struct task_t *current_task; 
struct task_t *task[NR_TASK]; 

static void create_task_tss(struct task_t *t, int (*task_fn)())
{
	struct tss_t *tss = &(t->tss);

	tss->esp0 = (int)t + PAGE_SIZE;
	tss->ss0 = 0x10;
	tss->eflags = 0x200;
	tss->esp = (int)t + PAGE_SIZE;
	tss->es = 0x10;
	tss->cs = 0x08;
	tss->ss = 0x10;
	tss->ds = 0x10;
	tss->es = 0x10;
	tss->fs = 0x17;
	tss->gs = 0x10;
	tss->eip = (uint32_t)task_fn;
	tss->ldt = _LDT(t->pid);
	
	/*加入gdt*/
	create_gdt_desc(GDT_TSS(t->pid), (uint32_t)(tss), 
		sizeof(struct task_t)-1, DA_386TSS);
}

static void create_task_ldt(struct task_t *t)
{
	struct desc *d = t->ldt;
	create_ldt_desc(d, 0, 0, 0);
	create_ldt_desc(d+1, (uint32_t)t->start_addr, 
		TASK_MEM_LEN, DA_DPL3|DA_32|DA_CR|DA_LIMIT_4K);
	create_ldt_desc(d+2, (uint32_t)t->start_addr, 
		TASK_MEM_LEN, DA_DPL3|DA_32|DA_DRW|DA_LIMIT_4K );
	
	/*加入gdt*/
	create_gdt_desc(GDT_LDT(t->pid), (uint32_t)d, 
		sizeof(struct desc)*3-1, DA_LDT);
}

struct task_t* create_task(int (*taskptr)())
{
	eflags_t eflag;
	disable_hwint(eflag);
	int pid = find_empty_process();

	assert(pid != -1);
	struct task_t *t = (struct task_t*)get_free_page();
	memset((void*)t, 0, PAGE_SIZE);
	t->pid = pid;
	t->ppid = 0;
	t->ticks = t->priority = 10;
	t->istask = 1;
	t->start_addr = 0;
	t->start_ticks = ticks;

	task[pid] = t;
	create_task_tss(t, taskptr);
	create_task_ldt(t);

	restore_hwint(eflag);
	return t;
}

struct task_t* create_task0()
{
	struct task_t *task0 = (struct task_t*)get_free_page();
	struct tss_t *tss = &(task0->tss);

	memset(task0, 0, sizeof(*task0));
	/* add to task struct */
	task[0] = task0;

	task0->ppid = 0;
	task0->pid = 0;
	task0->start_addr = 0;
	task0->ticks = task0->priority = 1;
	task0->status = RUN;
	task0->istask = 0;
	task0->start_ticks = ticks;
	create_task_ldt(task0);

	tss->eip = 0;
	tss->esp0 = (int)task0 + PAGE_SIZE;
	tss->ss0 = 0x10;
	tss->cs = 0x0F;
	tss->ds = 0x17;
	tss->es = 0x17;
	tss->gs = 0x17;
	tss->ss = 0x17;
	tss->fs = 0x17;
	tss->eflags = 0x200;
	tss->ldt = _LDT(0);

	create_gdt_desc(GDT_TSS(0), (uint32_t)tss, 
		sizeof(struct task_t) - 1, DA_386TSS);
	return task0;
}

int find_empty_process()
{
	int i;
	for(i=0; i<NR_TASK; i++){
		if(!task[i])
			return i;
	}
	return -1;
}


void sleep_on(struct task_t* t)
{
	if (t->pid == 0) {
		//hlt();
	} else {
		t->status = WAIT;
	}
	schedule();
}

void wake_up(struct task_t* t)
{
	t->status = RUN;
}

void* get_user_addr(struct task_t* t, void* addr)
{
	return (void*)((uint32_t)t->start_addr + (uint32_t)addr);
}

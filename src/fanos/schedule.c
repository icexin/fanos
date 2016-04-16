#include <fanos/mem.h>
#include <fanos/task.h>
#include <fanos/system.h>
#include <fanos/schedule.h>
#include <fanos/string.h>
#include <fanos/fmt.h>

uint32 ticks; /*系统嘀嗒数*/

void init_sched()
{
	memset(task, 0, NR_TASK * sizeof(struct task_t*));
	current_task = create_task0();
	assert(current_task->pid == 0);
}

void do_timer(int cpl)
{
	//log("timer interupt\n");
	if (cpl == 0) {
		//log("process %d(%x) in kernel interupted\n",
			//current_task->pid, (int)current_task);
	}
	current_task->cputime++;

	int i = 0;
	for(i=0; i<NR_TASK; i++){
		struct task_t* t = task[i];
		if(!t)continue;
		if (t->timeout > 0) {
			assert(t->status == WAIT);
			t->timeout -= 1000 / TIMER_HZ;
			if (t->timeout <= 0) {
				t->timeout = 0;
				wake_up(t);
			}
		}
	}

	schedule();
}


void schedule()
{
	//log("task %d in schedule\n", current_task->pid);
	struct task_t *t, *ready = task[0];
	int larger_ticks = 0;
	int i;

	for(i=1; i<NR_TASK; i++){
		t = task[i];
		if(!t || t == current_task)continue;
		if((t->ticks > larger_ticks) && (t->status == RUN)){
			larger_ticks = t->ticks;
			ready = t;
		}
	}

	if(larger_ticks <= 0){
		for(i = 1; i<NR_TASK; i++){
			t = task[i];
			if(!t || t == current_task)continue;
			t->ticks = t->priority;
			if(t->priority > larger_ticks && t->status == RUN){
				larger_ticks = t->priority;
				ready = t;
			}
		}
	}

	/*
	log("current process :%d, next process :%d\n",
		current_task->pid, ready->pid);
	*/

	if (ready->pid !=0 ) {
		ready->ticks--;
		assert(ready != current_task);
	}
	//log("task %d get ready\n", ready->pid);

	eflags_t eflags;
	disable_hwint(eflags);
	switch_to(ready->pid);
	//log("task %d leave schedule\n", current_task->pid);
	restore_hwint(eflags);
}

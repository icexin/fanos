#include <fanos/task.h>
#include <fanos/mem.h>
#include <fanos/debug.h>
#include <fanos/schedule.h>
#include <fanos/fmt.h>
#include <fanos/mem.h>

static void clean_up(struct task_t *t)
{
	task[t->pid] = 0;
	free_page((void*)t);
	log("task %d clean\n", t->pid);
}

void sys_exit(int status)
{
	int i;
	struct task_t *parent;
	struct task_t *t;

	if(current_task->pid == 0)
		panic("try to exit process 0\n");
	if(current_task->istask)
		panic("try to exit task\n");

	eflags_t eflag;
	disable_hwint(eflag);
	free_process_mem((void*)current_task->start_addr);

	current_task->exit_status = status;
	current_task->status = HANG;

	parent = task[current_task->ppid];

	/* parent is waiting */
	if(parent->status == WAIT)
		parent->status = RUN;

	/* make the sons of the current task to be the son of init */
	for(i=1; i<NR_TASK; i++){
		t = task[i];
		if(!t || current_task == t)continue;
		if(t->istask)continue; /* task do not join this */
		if(t->ppid == current_task->pid){
			t->ppid = 0;
		}
	}
	log("task %d exit\n", current_task->pid);
	restore_hwint(eflag);
	schedule();
}

int sys_waitpid(int pid, int *status)
{
	int i = 0;
	int ppid = current_task->pid;
	struct task_t *t;
	int flag = 0; /*找到标志*/
	while(1){
		for(i=1; i<NR_TASK; i++){
			t = task[i];
			if(!t || t==current_task)continue;
			if(t->istask)continue;
			if(t->ppid != ppid)continue;

			/* pid == -1 means that we wait for any son process */
			if(pid == -1 || t->pid == pid){
				flag = 1;
				if(t->status == HANG){
					if(status)
						put_fs_int(status,t->exit_status);
					clean_up(t);
					return 0;
				}
			}

		}/*end of for*/

		/* if we have son process but the son process is not hanging*/
		if(flag){
			current_task->status = WAIT;
		}

		if(!flag)
			return -1;
		/*等待子进程退出, 下个时间片再继续检查*/
		sleep_on(current_task);
	}
}

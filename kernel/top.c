#include <fanos/task.h>
#include <fanos/tty.h>
#include <fanos/time.h>
#include <fanos/kernel.h>
#include <stdio.h>

const int start_line = 2;
const int start_col = 55;

int task_top()
{
	static char buff[80];
	static size_t last_cputime[NR_TASK] = {0};
	static char status[] = {'R', 'W', 'H'};
	static size_t last_ticks = -2;
	int passed_ticks = 0;

	log("task top start with pid %d\n", current_task->pid);

	while (1) {
		snprintf(buff, 80, "%6s%6s%6s", "PID", "STAT", "%CPU");
		putline(start_line, start_col, "%s", buff);
		passed_ticks = sys_ticks() - last_ticks;
		last_ticks = sys_ticks();

		int i = 0;
		for(i=0; i<NR_TASK; i++){
			struct task_t* t = task[i];
			if(!t)continue;
			size_t task_cputime = t->cputime - last_cputime[i];
			last_cputime[i] = t->cputime;
			size_t pecent = task_cputime * 100 / passed_ticks;
			snprintf(buff, 80, "%6d%6c%6d", t->pid, status[t->status], pecent);
			putline(start_line + i + 1, start_col, "%s", buff);
		}
		sys_sleep(3000);
	}
	return 0;
}

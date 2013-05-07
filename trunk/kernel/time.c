#include <fanos/task.h>
#include <fanos/time.h>
#include <stdint.h>

extern unsigned long ticks;

long sys_ticks()
{
	return ticks;
}

void sys_sleep(size_t msec)
{
	if (msec < 0) {
		return;
	}
	current_task->timeout = msec;
	sleep_on(current_task);
}

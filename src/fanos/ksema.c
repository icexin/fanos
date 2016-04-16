#include <fanos/ksema.h>
#include <fanos/task.h>

int ksema_init(struct ksema* sem, unsigned int val)
{
	sem->value = val;
	waitq_init(&sem->waitq);
	return 0;
}

int ksema_down(struct ksema* sem)
{
	eflags_t eflags;
	disable_hwint(eflags);
	while (sem->value == 0) {
		waitq_append(&sem->waitq, current_task);
		sleep_on(current_task);
	}
	sem->value--;
	restore_hwint(eflags);
	return 0;
}

int ksema_up(struct ksema* sem)
{
	struct task_t *t;
	eflags_t eflags;
	disable_hwint(eflags);
	sem->value++;
	if (!waitq_isempty(&sem->waitq)) {
		waitq_pop(&sem->waitq, &t);
		wake_up(t);
	}
	restore_hwint(eflags);
	return 0;
}

int kmutex_init(kmutex* mutex)
{
	return ksema_init((struct ksema*)mutex, 1);
}

int kmutex_lock(kmutex* mutex)
{
	return ksema_down((struct ksema*)mutex);
}

int kmutex_unlock(kmutex* mutex)
{
	return ksema_up((struct ksema*)mutex);
}

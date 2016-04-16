#ifndef FANOS_KSEMA_H
#define FANOS_KSEMA_H

#include <fanos/waitq.h>
#include <fanos/system.h>
#include <fanos/task.h>

struct ksema {
	unsigned int value;
	struct waitq_t waitq;
};
typedef struct ksema kmutex;

int ksema_init(struct ksema* sem, unsigned int val);
int ksema_down(struct ksema* sem);
int ksema_up(struct ksema* sem);
int kmutex_lock(kmutex *mutex);
int kmutex_unlock(kmutex *mutex);
int kmutex_init(kmutex *mutex);

#endif

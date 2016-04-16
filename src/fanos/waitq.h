#ifndef FANOS_WAITQ_H
#define FANOS_WAITQ_H

#include <fanos/task.h>

#define MAX_QUEUE_LEN 32

typedef struct task_t* waitq_type;
struct waitq_t
{
	waitq_type content[MAX_QUEUE_LEN];
	int front;
	int tail;
	int len;
};

int waitq_init(struct waitq_t* q);
int waitq_append(struct waitq_t* q, waitq_type data);
int waitq_pop(struct waitq_t* q, waitq_type* data);
int waitq_isempty(struct waitq_t* q);

#endif

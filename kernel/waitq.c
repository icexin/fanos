#include <fanos/waitq.h>

int waitq_init(struct waitq_t* q)
{
	q->len = 0;
	q->front = q->tail = 0;
}

int waitq_append(struct waitq_t* q, waitq_type data)
{
	if (q->len >= MAX_QUEUE_LEN) {
		return -1;
	}
	q->content[q->tail] = data;
	q->tail = (q->tail + 1) % MAX_QUEUE_LEN;
	q->len++;
	return 0;
}

int waitq_pop(struct waitq_t* q, waitq_type* data)
{
	if (q->len <= 0) {
		return -1;
	}
	*data = q->content[q->front];
	q->front = (q->front + 1) % MAX_QUEUE_LEN;
	q->len--;
	return 0;
}

int waitq_isempty(struct waitq_t* q)
{
	return q->len == 0;
}

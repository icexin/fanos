#include <fanos/buffq.h>

int buffq_empty(struct buffq *buf)
{
	return buf->cnt <= 0;
}

int buffq_full(struct buffq *buf)
{
	return buf->cnt >= BUFFQ_LEN;
}

void buffq_init(struct buffq *buf)
{
	buf->head = buf->tail = 0;
	buf->cnt = 0;
}

char buffq_shift(struct buffq *buf)
{
	int i = buf->head;
	if(buf->cnt < 0){
		return -1;
	}
	buf->head = (i + 1) % BUFFQ_LEN;
	buf->cnt--;

	return buf->data[i];
}

int buffq_push(struct buffq *buf, char ch)
{
	int i = buf->tail;

	if(buf->cnt >= BUFFQ_LEN)
		return -1;

	buf->data[i] = ch;
	buf->tail = (i + 1) % BUFFQ_LEN;
	buf->cnt++;
	return 0;
}

char buffq_pop(struct buffq *buf)
{
	int i = buf->tail;
	if (buf->cnt <= 0) {
		return 0;
	}
	char ch = buf->data[i];
	buf->tail = (i + BUFFQ_LEN - 1) % BUFFQ_LEN;
	buf->cnt--;
	return ch;
}

int buffq_write(struct buffq* buf, char* str, int len)
{
	char* begin = str;
	while (len--) {
		if (!buffq_full(buf)) {
			buffq_push(buf, *str++);
		} else {
			break;
		}
	}
	return str - begin;
}

int buffq_read(struct buffq* buf, char* str, int len)
{
	char* begin = str;
	while (len--) {
		if (!buffq_empty(buf)) {
			*str++ = buffq_shift(buf);
		} else {
			break;
		}
	}
	return str - begin;
}

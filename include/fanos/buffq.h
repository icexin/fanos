#ifndef _BUFFQ_H
#define _BUFFQ_H

#define BUFFQ_LEN 512
struct buffq{
	char data[BUFFQ_LEN];
	int head;
	int tail;
	int cnt;
};

void buffq_init(struct buffq *buf);
int buffq_empty(struct buffq *buf);
int buffq_full(struct buffq *buf);
char buffq_shift(struct buffq *buf);
int buffq_push(struct buffq *buf, char ch);
char buffq_pop(struct buffq *buf);
int buffq_write(struct buffq* buf, char* str, int len);
int buffq_read(struct buffq* buf, char* str, int len);

#endif

#ifndef _TTY_H
#define _TTY_H

extern char* const videoram;

#define TTY_BUF_CNT  512

struct tty_buf{
	char data[TTY_BUF_CNT];
	int head;
	int tail;
	int cnt;
};

struct tty_t{
	struct tty_buf in_buf;
	struct tty_buf out_buf;
	struct tty_buf cooked_inbuf;
	char row;
	char col;
};

void init_tty();
void tty_clear();
int tty_write(char *str, int len);

int buf_empty(struct tty_buf *buf);
int buf_full(struct tty_buf *buf);
char buf_shift(struct tty_buf *buf);
int buf_push(struct tty_buf *buf, char ch);
#endif

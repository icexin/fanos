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
	char row;
	char col;
};

void init_tty();
void tty_clear();
int tty_write(char *str, int len);
#endif

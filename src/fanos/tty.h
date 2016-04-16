#ifndef FANOS_TTY_H
#define FANOS_TTY_H

#include <fanos/schedule.h>
#include <fanos/waitq.h>
#include <fanos/buffq.h>
#include <fanos/ksema.h>

#define MAX_ROW 25
#define MAX_COL 80

#define TAB_BLANK_NUM 4

#define screen(row,col) (*(videoram+((row)*MAX_COL+(col))*2))

extern char* const videoram;

struct tty_t{
	struct buffq in_buf;
	struct buffq out_buf;
	struct buffq cooked_inbuf;
	char row;
	char col;
	struct task_t *ttytask;
	struct ksema cooked_sem;
	kmutex cooked_mutex;
	struct ksema out_sem;
	kmutex out_mutex;
};

extern struct tty_t tty;

int sys_write(int fd, char *buf, int len);
int sys_read(int fd, char *buf, int len);
int tty_write(char *str, int len);
int tty_read(char *str, int len);
void init_tty();
void tty_clear();
void tty_update_cursor();
void tty_write_char(char ch);

int putline(int row, int col, const char* fmt, ...);

#endif

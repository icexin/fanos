#ifndef _TTY_H
#define _TTY_H

extern char* const videoram;

void init_tty();
void tty_clear();
int tty_write(char *str, int len);
#endif

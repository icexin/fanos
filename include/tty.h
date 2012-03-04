#ifndef _TTY_H
#define _TTY_H

typedef char* var_list;

void putchar(char ch);
void puts(char *base);
void init_tty();
char* number(char *str, int num, int base);
int vsprintf(char *str, const char *fmt, var_list args);
int sprintf(char *str, const char *fmt, ...);
int printf(const char *fmt, ...);
void tty_clear();
#endif

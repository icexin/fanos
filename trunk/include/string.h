#ifndef _STRING_H
#define _STRING_H
typedef char* var_list;

void putchar(char ch);
void puts(char *base);
void init_tty();
char* number(char *str, int num, int base);
int strlen(char *str);
int vsprintf(char *str, const char *fmt, var_list args);
int sprintf(char *str, const char *fmt, ...);
int printf(const char *fmt, ...);
#endif

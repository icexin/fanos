#ifndef _STDIO_H
#define _STDIO_H

#include <stdint.h>

void putchar(char ch);
void puts(char *base);
char *gets(char *str);
char *getline(char *str, int len);

char* number(char *str, unsigned int num, int base);
int vsnprintf(char *str, size_t n, const char *fmt, char* args);
int snprintf(char *str, size_t n, const char *fmt, ...);
int printf(const char *fmt, ...);

#endif

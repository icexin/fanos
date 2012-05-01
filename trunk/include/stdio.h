#ifndef _STDIO_H
#define _STDIO_H

void putchar(char ch);
void puts(char *base);
char *gets(char *str);
char *getline(char *str, int len);

char* number(char *str, unsigned int num, int base);
int vsprintf(char *str, const char *fmt, char* args);
int sprintf(char *str, const char *fmt, ...);
int printf(const char *fmt, ...);

#endif

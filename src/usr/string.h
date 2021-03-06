#ifndef _STRING_H
#define _STRING_H
#include <stdint.h>

char* strtok(char* s, const char* delim);
char *strcpy(char *dest, const char *src);
int strcmp(const char *s1, const char *s2);
int strlen(const char *str);
int isprint(char ch);
void memcpy(void *dest, const void *src, uint32 len);
void memset(void *dest, int to_set, uint32 n);
#endif

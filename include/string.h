#ifndef _STRING_H
#define _STRING_H
char *strcpy(char *dest, const char *src);
int strcmp(const char *s1, const char *s2);
int strlen(const char *str);
int isprint(char ch);
void memcpy(char *dest, const char *src, int len);
void memset(char *dest, int to_set, int n);
#endif

#ifndef _STRING_H
#define _STRING_H
char *strcpy(char *dest, char *src);
int strcmp(char *s1, char *s2);
int strlen(char *str);
int isprint(char ch);
void memcpy(char *dest, const char *src, int len);
void memset(char *dest, int to_set, int n);
#endif

#include <unistd.h>
#include <stdio.h>
#include <string.h>

char *gets(char *str)
{
	return getline(str, 128);
}

char *getline(char *str, int len)
{
	int cnt = read(0, str, len);
	str[cnt-1] = '\0';
	return str;
}


#include <string.h>

int strlen(char *str)
{
	int len = 0;
	while(*str++)len++;
	return len;
}


char * strcpy(char *dest, char *src)
{
	char *p = dest;
	while(*src)
	{
		*dest++ = *src++;
	}
	return p;
}

int isprint(char ch)
{
	return (ch >= '0' && ch <='9') || (ch >='a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

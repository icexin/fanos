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

void memcpy(char *dest, const char *src, int len)
{
	int i = 0;
	while(i++<len)*dest++ = *src++;
}

void memset(char *dest, int to_set, int n)
{
	while(n--)*dest++ = to_set;
}

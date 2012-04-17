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

int strcmp(char *s1, char *s2)
{
	while(*s1 && *s2){
		if(*s1 != *s2){
			break;
		}else{
			s1++;
			s2++;
		}
	}
	return *s1 - *s2;
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

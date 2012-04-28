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

inline void memcpy(char *dest, const char *src, int len)
{
	__asm__(
	"cld\n\t" \
	"rep movsb\n\t"
	::"c"(len),"S"(src),"D"(dest));
}

inline void memset(char *dest, int to_set, int n)
{
	__asm__(
	"cld\n\t" \
	"rep stosb\n\t"
	::"a"(to_set),"D"(dest),"c"(n));
}

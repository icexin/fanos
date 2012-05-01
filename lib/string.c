#include <string.h>

int strlen(const char *str)
{
	int len = 0;
	if(str == 0)
		return 0;
	while(*str++)++len;
	return len;
}


char * strcpy(char *dest, const char *src)
{
	if(src == 0 || dest == 0)
		return 0;
	char *p = dest;
	while(*src)
	{
		*dest++ = *src++;
	}
	return p;
}

int strcmp(const char *s1, const char *s2)
{
	if(s1 == 0 || s2 == 0)
		return 0;
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

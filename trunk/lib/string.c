#include <string.h>

int strlen(const char *str)
{
	int len = 0;
	if(str == 0)
		return 0;
	while(*str++)++len;
	return len;
}


char* strcpy(char *dest, const char *src)
{
	if(src == 0 || dest == 0)
		return 0;
	char *p = dest;
	while(*src)
	{
		*dest++ = *src++;
	}
	*dest = 0;
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

char* strtok(char* s, const char* delim)
{
	register char *spanp;
	register int c, sc;
	char *tok;
	static char *last;


	if (s == NULL && (s = last) == NULL)
		return (NULL);

	/*
	 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
cont:
	c = *s++;
	for (spanp = (char *)delim; (sc = *spanp++) != 0;) {
		if (c == sc)
			goto cont;
	}

	if (c == 0) {		/* no non-delimiter characters */
		last = NULL;
		return (NULL);
	}
	tok = s - 1;

	/*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */
	for (;;) {
		c = *s++;
		spanp = (char *)delim;
		do {
			if ((sc = *spanp++) == c) {
				if (c == 0)
					s = NULL;
				else
					s[-1] = 0;
				last = s;
				return (tok);
			}
		} while (sc != 0);
	}
	/* NOTREACHED */
}

inline void memcpy(void *dest, const void *src, size_t len)
{
	__asm__(
	"cld\n\t" \
	"rep movsb\n\t"
	::"c"(len),"S"(src),"D"(dest));
}

inline void memset(void *dest, int to_set, size_t n)
{
	__asm__(
	"cld\n\t" \
	"rep stosb\n\t"
	::"a"(to_set),"D"(dest),"c"(n));
}

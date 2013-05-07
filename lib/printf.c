#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>

static inline int mon_isdigit(int c)
{
	    return c >= '0' && c <= '9';
}

/* from BSD ppp sources */
int vsnprintf(char *buf, size_t buflen, const char *fmt, va_list args)
{
	int c, n;
	int width, prec, fillch;
	int base, len, neg, is_long;
	unsigned long val = 0;
	const char *f;
	char *str, *buf0;
	char num[32];
	static const char hexchars[] = "0123456789abcdef";

	buf0 = buf;
	--buflen;
	while (buflen > 0) {
		for (f = fmt; *f != '%' && *f != 0; ++f) 
			;
		if (f > fmt) {
			len = f - fmt;
			if (len > buflen)
				len = buflen;
			memcpy(buf, fmt, len);
			buf += len;
			buflen -= len;
			fmt = f;
		}
		if (*fmt == 0)
			break;
		c = *++fmt;
		width = prec = 0;
		fillch = ' ';
		if (c == '0') {
			fillch = '0';
			c = *++fmt;
		}
		if (c == '*') {
			width = va_arg(args, int);
			c = *++fmt;
		} else {
			while (mon_isdigit(c)) {
				width = width * 10 + c - '0';
				c = *++fmt;
			}
		}
		if (c == '.') {
			c = *++fmt;
			if (c == '*') {
				prec = va_arg(args, int);
				c = *++fmt;
			} else {
				while (mon_isdigit(c)) {
					prec = prec * 10 + c - '0';
					c = *++fmt;
				}
			}
		}
		/* modifiers */
		is_long = 0;
		switch(c) {
			case 'l':
				is_long = 1;
				c = *++fmt;
				break;
			default:
				break;
		}
		str = 0;
		base = 0;
		neg = 0;
		++fmt;
		switch (c) {
			case 'd':
				if (is_long)
					val = va_arg(args, long);
				else
					val = va_arg(args, int);
				if ((long)val < 0) {
					neg = 1;
					val = -val;
				}
				base = 10;
				break;
			case 'o':
				if (is_long)
					val = va_arg(args, unsigned long);
				else
					val = va_arg(args, unsigned int);
				base = 8;
				break;
			case 'x':
			case 'X':
				if (is_long)
					val = va_arg(args, unsigned long);
				else
					val = va_arg(args, unsigned int);
				base = 16;
				break;
			case 'p':
				val = (unsigned long) va_arg(args, void *);
				base = 16;
				neg = 2;
				break;
			case 's':
				str = va_arg(args, char *);
				break;
			case 'c':
				num[0] = va_arg(args, int);
				num[1] = 0;
				str = num;
				break;
			default:
				*buf++ = '%';
				if (c != '%')
					--fmt;		/* so %z outputs %z etc. */
				--buflen;
				continue;
		}
		if (base != 0) {
			str = num + sizeof(num);
			*--str = 0;
			while (str > num + neg) {
				*--str = hexchars[val % base];
				val = val / base;
				if (--prec <= 0 && val == 0)
					break;
			}
			switch (neg) {
				case 1:
					*--str = '-';
					break;
				case 2:
					*--str = 'x';
					*--str = '0';
					break;
			}
			len = num + sizeof(num) - 1 - str;
		} else {
			len = strlen(str);
			if (prec > 0 && len > prec)
				len = prec;
		}
		if (width > 0) {
			if (width > buflen)
				width = buflen;
			if ((n = width - len) > 0) {
				buflen -= n;
				for (; n > 0; --n)
					*buf++ = fillch;
			}
		}
		if (len > buflen)
			len = buflen;
		memcpy(buf, str, len);
		buf += len;
		buflen -= len;
	}
	*buf = 0;
	return buf - buf0;
}

char* number(char *str, unsigned int num, int base)
{
	static int tmp[36];
	int i; 
	char *p = str;
	const char *digits="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int n = 0;
	if(base == 10 && (int)num < 0){
		*str++ = '-';
		num = (unsigned int)(-(int)num);
	}
		
	if(num == 0)
		*str++ = '0';
	for(i=0; num; num/=base){
		tmp[i++] = num % base;
	}
	while(i--){
		*str++ = digits[tmp[i]];
	}
	*str = 0;
	return p;
}

int vsprintf(char *str, const char *fmt, va_list args)
{
	char *p = str;
	static char tmp[256];

	for(; *fmt; fmt++)
	{
		if(*fmt != '%')
		{
			*str++ = *fmt;
			continue;
		}
		fmt++;

		switch(*fmt)
		{
			case 'c':
				*str++ = *((char*)args);
				args += 4;
				break;
			case 's':
				strcpy(str, *(char**)args);
				str += strlen(*(char**)args);
				args += 4;
				break;
			case 'd':
				number(tmp, *(unsigned int *)args, 10);
				strcpy(str, tmp);
				str += strlen(tmp);
				args += 4;
				break;
			case 'x':
				number(tmp, *(unsigned int *)args, 16);
				strcpy(str, tmp);
				str += strlen(tmp);
				args += 4;
		}
	}
	*str = 0;
	return str - p;
}

int snprintf(char *str, size_t n, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	return vsnprintf(str, n, fmt, ap);
}

int fprintf(int fd, const char *fmt, ...)
{
	static char tmp[256];
	int n;
	va_list ap;
	va_start(ap, fmt);
	n = vsnprintf(tmp, 256, fmt, ap);
	write(fd, tmp, n);
	return n;
}
	
int printf(const char *fmt, ...)
{
	static char tmp[256];
	int n;
	va_list ap;
	va_start(ap, fmt);
	n = vsnprintf(tmp, 256, fmt, ap);
	va_end(ap);

	write(STDOUT_FILENO, tmp, n);
	return n;
}

void puts(char * base)
{
	write(STDOUT_FILENO, base, strlen(base));
	putchar('\n');
}

void putchar(char ch)
{
	printf("%c", ch);
}

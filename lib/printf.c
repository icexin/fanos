#include <stdio.h>
#include <unistd.h>

typedef char *var_list;

char* number(char *str, unsigned int num, int base)
{
	int tmp[36];
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

int vsprintf(char *str, const char *fmt, var_list args)
{
	char *p = str;
	char tmp[256];

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
	return str - p;
}

int sprintf(char *str, const char *fmt, ...)
{
	var_list ap = (var_list)((char*)&fmt + 4);
	return vsprintf(str, fmt, ap);
}

int fprintf(int fd, const char *fmt, ...)
{
	static char tmp[256];
	int n;
	var_list ap = (var_list)((char*)&fmt + 4);
	n = vsprintf(tmp, fmt, ap);
	write(fd, tmp, n);
	return n;
}
	
int printf(const char *fmt, ...)
{
	static char tmp[256];
	int n;
	var_list ap = (var_list)((char*)&fmt + 4);
	n = vsprintf(tmp, fmt, ap);
	write(1, tmp, n);
	return n;
}


void puts(char * base)
{
	write(1, base, strlen(base));
	putchar('\n');
}

void putchar(char ch)
{
	printf("%c", ch);
}

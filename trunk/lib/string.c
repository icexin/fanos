#include <global.h>
#include <string.h>
#include <protect.h>

static char* const videoram = (char *const)0xB8000;
static unsigned char pos_row = 0;
static unsigned char pos_col = 0;

int strlen(char *str)
{
	int len = 0;
	while(*str++)len++;
	return len;
}

char* number(char *str, int num, int base)
{
	int tmp[36];
	int i; 
	char *p = str;
	const char *digits="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if(num < 0)
	{
		*str++ = '-';
		num = -num;
	}
	for(i=0; num; num/=base)
	{
		tmp[i++] = num % base;
	}
	while(i--)
	{
		*str++ = digits[tmp[i]];
	}
	*str = 0;
	return p;
}

void init_tty()
{
	disable_int();
	out_byte(0x3D4, 0xC);
	out_byte(0x3D5, 0);

	out_byte(0x3D4, 0xD);
	out_byte(0x3D5, 0);
	enable_int();
}

	

void puts(char * base)
{
	while(*base)
	{
		putchar(*(base++));
	}
	putchar('\n');
}

void putchar(char ch)
{
	if(ch == '\n' || pos_col == 79)
	{
		pos_row++;
		pos_col = 0;
	}

	if(ch != '\n')
	{
		videoram[(pos_row * 80 + pos_col++) * 2] = ch;
	}
	disable_int();
	out_byte(0x3D4, 0x0E);
	out_byte(0x3D5, (pos_row * 80 + pos_col) >> 8);

	out_byte(0x3D4, 0x0F);
	out_byte(0x3D5, (pos_row * 80 + pos_col) & 0xFF);

	enable_int();
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

int vsprintf(char *str, const char *fmt, var_list args)
{
	char *p = str;
	char tmp[256];
	int  base = 0;

	while(*fmt)
	{
		if(*fmt != '%')
		{
			*str++ = *fmt;
			continue;
		}
		fmt++;

		switch(*fmt++)
		{
			case 'c':
				*str++ = *((char*)args);
				args += 1;
				break;
			case 's':
				break;
			case 'd':
				number(tmp, *(int *)args, 10);
				strcpy(str, tmp);
				str += strlen(tmp);
				break;
			case 'x':
				number(tmp, *(int *)args, 16);
				strcpy(str, tmp);
				str += strlen(tmp);
		}
	}
	return str - p;
}



int sprintf(char *str, const char *fmt, ...)
{
	var_list ap = (var_list)((char*)&fmt + 4);
	return vsprintf(str, fmt, ap);
}

int printf(const char *fmt, ...)
{
	char tmp[256];
	var_list ap = (var_list)((char*)&fmt + 4);
	vsprintf(tmp, fmt, ap);
	puts(tmp);
}

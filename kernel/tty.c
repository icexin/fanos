#include <tty.h>
#include <string.h>
#include <unistd.h>

static char* const videoram = (char *const)0xB8000;
static unsigned char pos_row = 0;
static unsigned char pos_col = 0;

int sys_write(int fd, char *buf, int len)
{
	return tty_write(buf, len);
}


int tty_write(char *str, int len)
{
	int i;
	for(i=0; i<len; i++)
	{
		if(str[i] == '\n' || pos_col == 79)
		{
			pos_row++;
			pos_col = 0;
		}

		if(str[i] != '\n')
		{
			videoram[(pos_row * 80 + pos_col++) * 2] = str[i];
		}
	}

	if(pos_row > 80)
	{
		pos_row = 0;
	}
	disable_int();
	out_byte(0x3D4, 0x0E);
	out_byte(0x3D5, (pos_row * 80 + pos_col) >> 8);

	out_byte(0x3D4, 0x0F);
	out_byte(0x3D5, (pos_row * 80 + pos_col) & 0xFF);

	enable_int();

}


char* number(char *str, int num, int base)
{
	int tmp[36];
	int i; 
	char *p = str;
	const char *digits="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	if(num < 0){
		*str++ = '-';
		num = -num;
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

void init_tty()
{
	disable_int();
	out_byte(0x3D4, 0xC);
	out_byte(0x3D5, 0);

	out_byte(0x3D4, 0xD);
	out_byte(0x3D5, 0);
	enable_int();
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
				number(tmp, *(int *)args, 10);
				strcpy(str, tmp);
				str += strlen(tmp);
				args += 4;
				break;
			case 'x':
				number(tmp, *(int *)args, 16);
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

int printf(const char *fmt, ...)
{
	char tmp[256];
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
	write(1, &ch, 1);
}

void tty_clear()
{
   int i;
   for (i=0;i<80*25;i++) {
       videoram[i*2] = ' ';
   }
}

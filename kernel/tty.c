#include <tty.h>
#include <string.h>
#include <unistd.h>
#include <system.h>

#define MAX_ROW 25
#define MAX_COL 80

#define screen(row,col) (*(videoram+((row)*MAX_COL+(col))*2))
char* const videoram = (char *const)0xB8000;
static unsigned char pos_row = 0;
static unsigned char pos_col = 0;
extern int rs_write(char *buf, int len);

int sys_write(int fd, char *buf, int len)
{
	if(fd == 1)
		return rs_write(buf, len);
	else
		return tty_write(buf, len);
	
}


int tty_write(char *str, int len)
{
	int i, j, k;
	for(i=0; i<len; i++){

		if(str[i] == '\n' || pos_col == MAX_COL){
			if(pos_col == MAX_COL){
				i--;
			}
			pos_row++;
			pos_col = 0;
			continue;
		}

		/*如果要写的行超出最大行，则所有行依次上移*/
		if(pos_row >= MAX_ROW){
			for(j=0; j<MAX_ROW-1; j++){
				for(k=0; k<MAX_COL; k++){
					screen(j,k) = screen(j+1,k);
				}
			}

			/*清最后一行*/
			for(j=0; j<MAX_COL; j++){
				screen(MAX_ROW-1, j) = ' ';
			}

			pos_row = MAX_ROW - 1;
			pos_col = 0;
		}

		screen(pos_row, pos_col++) = str[i];
	}

	cli();
	out_byte(0x3D4, 0x0E);
	out_byte(0x3D5, (pos_row * 80 + pos_col) >> 8);

	out_byte(0x3D4, 0x0F);
	out_byte(0x3D5, (pos_row * 80 + pos_col) & 0xFF);

	sti();

}


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

void init_tty()
{
	out_byte(0x3D4, 0xC);
	out_byte(0x3D5, 0);

	out_byte(0x3D4, 0xD);
	out_byte(0x3D5, 0);
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
	write(1, &ch, 1);
}

void tty_clear()
{
   int i;
   for (i=0;i<80*25;i++) {
       videoram[i*2] = ' ';
   }
}

#include <global.h>
#include <string.h>
#include <protect.h>

static char* const videoram = (char *const)0xB8000;
static unsigned char pos_row = 0;
static unsigned char pos_col = 0;

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

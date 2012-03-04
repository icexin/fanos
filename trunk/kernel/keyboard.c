#include <tty.h>
unsigned char keymap;

void handle()
{
	int ch;
	ch = in_byte(0x60);
	printf("%x,", ch);
}

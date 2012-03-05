#include <tty.h>
unsigned char keymap;
int choice = 1;


void handle()
{
	int ch;
	ch = in_byte(0x60);
	printf("%x,", ch);
}

void taska()
{
	while(1)
	{
		putchar('a');
	}
}

void taskb()
{
	while(1)
	{
		putchar('b');
	}
}

void handle_timer()
{
	if(choice == 0)
	{
		choice = !choice;
		__asm__("ljmp $0x28, $0");
	}
	else
	{
		choice = !choice;
		__asm__("ljmp $0x30, $0");
	}

	
}

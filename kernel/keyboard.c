#include <tty.h>
unsigned char keymap;
int choice = 1;


void handle()
{
	int ch;
	ch = in_byte(0x60);
	printf("%x,", ch);
}

int taska()
{
	while(1)
	{
		putchar('a');
	}
}

int taskb()
{
	while(1)
	{
		putchar('b');
	}
}

int taskc()
{
	while(1)
	{
		putchar('c');
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

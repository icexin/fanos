#include <tty.h>
unsigned char keymap;
int choice = 1;

void delay(int n)
{
	int i;
	while(n--)
		for(i=0; i<100000; i++);
}

void handle()
{
	char ch = '#';
	in_byte(0x60);
	int i;
	for(i=0; i<50; i++)
		rs_write(&ch, 1);
}

int taska()
{
	while(1)
	{
		putchar('a');
		delay(100);
	}
}

int taskb()
{
	while(1)
	{
		putchar('b');
		delay(100);
	}
}

int taskc()
{
	while(1)
	{
		putchar('c');
		delay(100);
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

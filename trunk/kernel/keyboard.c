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
	in_byte(0x60);
	int i;
	for(i=0; i<50; i++)
		printf("#");
}

int taska()
{
	while(1)
	{
		__asm__ __volatile__("mov $0, %eax");
	}
}

int taskb()
{
	while(1)
	{
		__asm__ __volatile__("mov $1, %eax");
	}
}

int taskc()
{
	while(1)
	{
		__asm__ __volatile__("mov $2, %eax");
	}
}


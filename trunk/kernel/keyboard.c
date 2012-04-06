#include <tty.h>
#include <time.h>

unsigned char keymap;

void delay(int n)
{
	int i;
	while(n--)
		for(i=0; i<100000; i++);
}

void handle()
{
	static int i=0;
	in_byte(0x60);
	log("%d,", i++);
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


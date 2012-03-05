#include <tty.h>
unsigned char keymap;
int choice = 1;


void handle()
{
	int ch;
	ch = in_byte(0x60);
	printf("%x,", ch);
}
void handle_timer()
{
	if(choice == 0)
	{
	//	__asm__("ljmp $0x28, $0");
	}
	else
	{
		//__asm__("ljmp $0x30, $0");
	}

	choice = !choice;
	putchar('#');
	
}

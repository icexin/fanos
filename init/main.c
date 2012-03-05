#include <kernel.h>
void move_to_user();
int main(void *mbd, unsigned int magic)
{
	init_gdt();
	init_idt();
	init_timer();
	tty_clear();
	move_to_user();
	while(1);
	return 0;
}

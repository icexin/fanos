#include <schedule.h>

void move_to_user();
int main(void *mbd, unsigned int magic)
{

	tty_clear();
	init_gdt();
	init_idt();
	init_sched();
	move_to_user();
	return 0;
}

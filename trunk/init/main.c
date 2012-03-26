#include <schedule.h>

void move_to_user();
void rs_init();

int main(void *mbd, unsigned int magic)
{

	rs_init();
	init_gdt();
	init_idt();
	init_sched();
	move_to_user();
	return 0;
}


#include <schedule.h>
#include <kernel.h>
#include <mem.h>
#include <unistd.h>
#include <system.h>

void move_to_user();
void rs_init();

int main(void *mbd, unsigned int magic)
{

	rs_init();
	init_gdt();
	init_idt();
	init_sched();

	tty_clear();	
	printf("Kernel begin\n");
	move_to_user();
	return 0;
}


#include <schedule.h>
#include <kernel.h>
#include <mem.h>
#include <unistd.h>

void move_to_user();
void rs_init();

int main(void *mbd, unsigned int magic)
{

	rs_init();
	init_gdt();
	init_idt();
	init_sched();

	tty_clear();	
	void *page = get_free_page();
	fprintf(STDERR_FILENO, "page=%x\n", (int)page);
	page = get_free_page();
	fprintf(STDERR_FILENO, "page=%x\n", (int)page);
	free_page(page);
	page = get_free_page();
	fprintf(STDERR_FILENO, "page=%x\n", (int)page);
	move_to_user();
	return 0;
}


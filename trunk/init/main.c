#include <schedule.h>
#include <kernel.h>
#include <mem.h>
#include <unistd.h>
#include <system.h>
#include <hd.h>

void move_to_user();
void rs_init();
char hd_buf[512] = {0};

int main(void *mbd, unsigned int magic)
{

	rs_init();
	init_gdt();
	init_idt();

	tty_clear();	
	sti();
	printf("Kernel begin\n");

	init_sched();

	hd_init();
	log("hd init\n");
	if(hd_read(0, hd_buf)){
		int i;
		for(i=0; i<512; i++){
			printf("%x,", (unsigned char)hd_buf[i]);
		}
	}

	move_to_user();
	return 0;
}


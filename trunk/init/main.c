#include <schedule.h>
#include <kernel.h>
#include <mem.h>
#include <unistd.h>
#include <system.h>
#include <hd.h>
#include <multiboot.h>
#include <fs.h>
#include <debug.h>

void rs_init();
char hd_buf[512] = {0};
extern char _end;


int main(void *mbd, unsigned int magic)
{
	multiboot_info_t *info;
	info = mbd;
	multiboot_module_t *modu = info->mods_addr;
	ramfs_start = modu->mod_start;
	ramfs_end = modu->mod_end;

	init_gdt();
	init_idt();
	init_mem();
	rs_init();
	init_sched();
	
	printk("*******************Kernel begin******************\n");

	tty_clear();	
	init_tty();

	out_byte(0x21, in_byte(0x21)&~0x01);
	move_to_user();
	int pid = fork();
	if(!pid){
		printf("start shell\n");
		exec("shell");		
	}
	for(;;)wait(0);
	return 0;
}


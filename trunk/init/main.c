#include <schedule.h>
#include <kernel.h>
#include <mem.h>
#include <unistd.h>
#include <system.h>
#include <hd.h>
#include <multiboot.h>
#include <fs.h>

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

	tty_clear();	
	printf("Kernel begin\n");

	

	char buf[1024];
	struct inode *node;
	node = fs_open("hello");
	int len;
	len = fs_read(node, buf);
	puts(buf);

	init_sched();

	move_to_user();
	return 0;
}


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
	rs_init();
	init_gdt();
	init_idt();

	tty_clear();	
	/*
	printf("Kernel begin\n");
	printf("end=%x\n", &_end);

	multiboot_module_t *modu = info->mods_addr;
	ramfs_start = modu->mod_start;
	ramfs_end = modu->mod_end;
	
	printf("ramdisk size:%d, start:%x, end:%x\n", \
		ramfs_end - ramfs_start, ramfs_start, ramfs_end);

//	read_sblk(0);

//	fs_open("hello");
*/
	init_sched();

//	hd_init();
//	log("hd init\n");
/*
	if(hd_read(0, hd_buf)){
		int i;
		for(i=0; i<512; i++){
			printf("%x,", (unsigned char)hd_buf[i]);
		}
	}
*/
	move_to_user();
	return 0;
}


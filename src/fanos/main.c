#include <fanos/task.h>
#include <fanos/schedule.h>
#include <fanos/mem.h>
#include <fanos/system.h>
#include <fanos/fs.h>
#include <fanos/hd.h>
#include <fanos/time.h>
#include <fanos/ksema.h>
#include <fanos/debug.h>
#include <fanos/multiboot.h>
#include <fanos/fmt.h>
#include <fanos/tty.h>
#include <fanos/serial.h>
#include <fanos/syscall.h>

static int fork()
{
	syscall0(SYS_FORK);
}

static int wait(int *status)
{
	syscall2(SYS_WAITPID, -1, status);
}

static int exec(char *name)
{
	char* argv[2] = {name, NULL};
	syscall2(SYS_EXEC, name, argv);
}

int main(void *mbd, unsigned int magic)
{
	multiboot_info_t *info;
	info = mbd;
	multiboot_module_t *modu = (multiboot_module_t*)info->mods_addr;
	ramfs_start = (void*)modu->mod_start;
	ramfs_end = (void*)modu->mod_end;

	cli();
	init_gdt();
	init_pic();
	init_mem();
	rs_init();
	init_sched();
	init_tty();
	mount();

	log("*******************Kernel begin******************\n");
	move_to_user();
	int pid = fork();
	if (!pid) {
		exec("/shell");
	}
	for(;;)wait(NULL);
	return 0;
}

/*
void print_mem(multiboot_info_t *info)
{
	multiboot_memory_map_t *mmap;

	log("mmap_addr = 0x%x,mmap_length = 0x%x\n",
			(unsigned) info->mmap_addr,(unsigned) info->mmap_length);

	for (mmap = (multiboot_memory_map_t *)info->mmap_addr;
			(unsigned long) mmap <info->mmap_addr + info->mmap_length;
			mmap =(multiboot_memory_map_t *) ((unsigned long) mmap
				+mmap->size + sizeof (mmap->size)))

		printf(" size = 0x%x,base_addr = 0x%x%x,"
				" length =%dK, type = 0x%x\n",
				mmap->size,
				(uint32)(mmap->addr >> 32),
				(uint32)(mmap->addr & 0xFFFFFFFF),
				(uint32)(mmap->len / 1024),
				mmap->type);
}
*/

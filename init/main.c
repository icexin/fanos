#include <fanos/task.h>
#include <fanos/schedule.h>
#include <fanos/kernel.h>
#include <fanos/mem.h>
#include <fanos/system.h>
#include <fanos/fs.h>
#include <fanos/hd.h>
#include <fanos/time.h>
#include <fanos/ksema.h>
#include <fanos/debug.h>
#include <unistd.h>

#include <multiboot.h>

void rs_init();
char hd_buf[512] = {0};

int global_cnt = 0;

extern int task_top();

int run0()
{
	log("task 0 start with pid %d\n", current_task->pid);
	while (1) {
		putline(1, 0, "0(%d): number is %d", sys_ticks(), global_cnt++);
	}
	return 0;
}

int run1()
{
	log("task 1 start with pid %d\n", current_task->pid);
	while (1) {
		putline(2, 0, "1(%d): number is %d", sys_ticks(), global_cnt++);
	}
	return 0;
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

	//create_task(run0);
	//create_task(run1);
	//create_task(task_top);

	log("*******************Kernel begin******************\n");
	move_to_user();
	print_mem(info);
	int pid = fork();
	if (!pid) {
		execl("/shell", NULL);
	}
	for(;;)wait(NULL);
	return 0;
}

void print_mem(multiboot_info_t *info)
{
	multiboot_memory_map_t *mmap;

	printf ("mmap_addr = 0x%x,mmap_length = 0x%x\n",
			(unsigned) info->mmap_addr,(unsigned) info->mmap_length);

	for (mmap = (multiboot_memory_map_t *)info->mmap_addr;
			(unsigned long) mmap <info->mmap_addr + info->mmap_length;
			mmap =(multiboot_memory_map_t *) ((unsigned long) mmap
				+mmap->size + sizeof (mmap->size)))

		printf(" size = 0x%x,base_addr = 0x%x%x,"
				" length =%dK, type = 0x%x\n",
				mmap->size,
				(uint32_t)(mmap->addr >> 32),
				(uint32_t)(mmap->addr & 0xFFFFFFFF),
				(uint32_t)(mmap->len / 1024),
				mmap->type);
}

#include <kernel.h>
#include <mem.h>
#include <unistd.h>
#include <fs.h>

extern char _kend;

char page_map[8*1024]; //32M
char process_mem_map[32];

void *get_free_page()
{
	int i;
	for(i=FREE_MEM_START>>12; i<FREE_MEM_END>>12; i++){
		if(!page_map[i]){
			page_map[i] = 1;
			return (void*)(i<<12);
		}
	}
}

void *alloc_process_mem()
{
	int i=0;
	for(i=12; i<32; i++){
		if(!process_mem_map[i]){
			process_mem_map[i] = 1;
			return (void*)(i<<20);
		}
	}
}

void free_process_mem(void *mem)
{
	int i = (int)mem >> 20;
	assert(i>=12 && i<32);
	page_map[i] = 0;
}


void free_page(void *page)
{
	int i = ((int)page -  FREE_MEM_START) >> 12;
	fprintf(STDOUT_FILENO, "[mem] free page:%x, index:%d\n", page, i);
	assert((int)page >= FREE_MEM_START && (int)page <= FREE_MEM_END);
	page_map[i] = 0;
}

static void use_page(void *start, void *end)
{
	int s, e;
	int i;
	s = ((int)start & 0xFFFFF000) >> 12;
	e = ((int)end & 0xFFFFF000) >> 12;
	for(i=s; i<=e; i++){
		page_map[i] = 1;
	}
}


void init_mem()
{
	/*2M以下不使用*/	
	use_page(0, FREE_MEM_START);
	use_page(ramfs_start, ramfs_end);
		
}

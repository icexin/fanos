#include <fanos/kernel.h>
#include <fanos/mem.h>
#include <fanos/fs.h>
#include <stdint.h>
#include <unistd.h>

extern char _kend;

char page_map[8*1024]; //32M
char process_mem_map[32];

void *get_free_page()
{
	int i;
	for(i=(FREE_MEM_START>>12); i<FREE_MEM_END>>12; i++){
		if(!page_map[i]){
			page_map[i] = 1;
			return (void*)(i<<12);
		}
	}
	return NULL;
}

void free_page(void *page)
{
	int i = (int)page >> 12;
	assert((int)page >= FREE_MEM_START && (int)page <= FREE_MEM_END);
	page_map[i] = 0;
}

void *alloc_process_mem()
{
	int i=0;
	for(i=12; i<32; i+=2){
		if(!process_mem_map[i]){
			process_mem_map[i] = 1;
			process_mem_map[i+1] = 1;
			return (void*)(i<<20);
		}
	}
	return NULL;
}

void free_process_mem(void *mem)
{
	int i = (int)mem >> 20;
	assert(i>=12 && i<32);
	process_mem_map[i] = 0;
	process_mem_map[i+1] = 0;
}

static void use_page(void *start, void *end)
{
	int s, e;
	int i;
	s = ((uint32_t)start & 0xFFFFF000) >> 12;
	e = ((uint32_t)end & 0xFFFFF000) >> 12;
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

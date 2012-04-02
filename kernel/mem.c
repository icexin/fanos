#include <kernel.h>
#include <mem.h>
#include <unistd.h>

char page_map[8 * 1024]={0}; //32M

void *get_free_page()
{
	int i;
	for(i=FREE_MEM_START>>12; i<FREE_MEM_END>>12; i++){
		if(!page_map[i]){
			page_map[i] = 1;
//			fprintf(STDOUT_FILENO, "[mem] get page:%x, index:%d\n", i<<12, i);
			return (void*)(i<<12);
		}
	}
}

void free_page(void *page)
{
	int i = (int)page >> 12;
	fprintf(STDOUT_FILENO, "[mem] free page:%x, index:%d\n", page, i);
	assert((int)page >= FREE_MEM_START && page <= FREE_MEM_END);
	page_map[i] = 0;
}

void init_page()
{
	int i = 0;

	//0-2M为内核空间
	for(i=0; i<FREE_MEM_START>>12; i++){
		page_map[i] = 1;
	}
}

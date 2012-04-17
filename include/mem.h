#ifndef _MEM_H
#define _MEM_H

#define PAGE_SIZE (4*1024)
#define FREE_MEM_START (2*1024*1024) 
#define FREE_MEM_END   (12*1024*1024) 

#define TASK_SIZE  PAGE_SIZE


void *alloc_process_mem();
void *get_free_page();
void free_page(void *page);
void init_mem();

#endif

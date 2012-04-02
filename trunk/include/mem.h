#ifndef _MEM_H
#define _MEM_H

#define PAGE_SIZE (4*1024)
#define FREE_MEM_START (2*1024*1024) 
#define FREE_MEM_END   (32*1024*1024) 
void *get_free_page();
void free_page(void *page);
void init_page();

#endif

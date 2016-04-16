#ifndef _MEM_H
#define _MEM_H

#define PAGE_SIZE (4*1024)
#define FREE_MEM_START (2*1024*1024)
#define FREE_MEM_END   (12*1024*1024)

// 0-2M为内核空间, 2-12M为进程结构体空间 12-32M为进程栈空间

#define PROCESS_MEM_LEN 0x1FF //ldt用的是4k粒度,所以进程栈空间为2M
#define TASK_MEM_LEN 0x1FF //ldt用的是4k粒度,所以任务栈空间为2M

void *alloc_process_mem();
void *get_free_page();
void free_page(void *page);
void init_mem();
void free_process_mem(void *mem);

#endif

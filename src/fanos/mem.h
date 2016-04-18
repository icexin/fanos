#ifndef FANOS_MEM_H
#define FANOS_MEM_H

#define PAGE_SIZE (4*1024)
#define FREE_MEM_START (2*1024*1024)
#define FREE_MEM_END   (12*1024*1024)

// addr到page首地址的转换
#define addr_to_page(x) ((unsigned int)(x) & 0xFFFFF000)

// page的首地址到page的索引
#define page_idx(x) ((unsigned int)(x) >> 12)

// page的索引到page的首地址
#define idx_to_page(x) (void*)((x) << 12)

// 0-2M为内核空间, 2-12M为进程结构体空间 12-32M为进程栈空间

#define PROCESS_MEM_LEN 0x1FF //ldt用的是4k粒度,所以进程栈空间为2M
#define TASK_MEM_LEN 0x1FF //ldt用的是4k粒度,所以任务栈空间为2M

void *alloc_process_mem();
void *get_free_page();
void free_page(void *page);
void init_mem();
void free_process_mem(void *mem);

#endif

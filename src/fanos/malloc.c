#include <fanos/string.h>

extern void* get_free_page();

#define DESC_DIR_SIZE 8

typedef union bucket_block {
  union bucket_block* next;
  char addr[0];
} bucket_block_t;

typedef struct bucket_desc {
  // 链表的next域，指向下一个同样size的bucket
  struct bucket_desc* next;

  // 可用的block链表
  bucket_block_t* free;

} bucket_desc_t;

// 16, 32...2048, 共8个类型
static bucket_desc_t* bucket_dir[DESC_DIR_SIZE];

static bucket_desc_t* new_bucket(int size)
{
  bucket_desc_t* desc = get_free_page();
  memset(desc, 0, 4096);

  char* header = (char*)desc + sizeof(bucket_desc_t);
  bucket_block_t* curr = (bucket_block_t*)header;
  char* p = NULL;
  for (p=header+size; p+size < (char*)desc+4096; p+=size) {
    curr->next = (bucket_block_t*) p;
    curr = curr->next;
  }
  curr->next = NULL;

  desc->free = (bucket_block_t*)header;
  return desc;
}

void kmalloc_init()
{
  int i = 0;
  for (i=0; i<DESC_DIR_SIZE; i++) {
    bucket_dir[i] = new_bucket(16*(1<<i));
  }
}

void* kmalloc(int size)
{
  int i = 0;
  int bkt_size = 0;
  bucket_desc_t* bkt = NULL;
  for (i=0; i<DESC_DIR_SIZE; i++) {
    bkt = bucket_dir[i];
    bkt_size = 16*(1<<i);
    if (size < bkt_size) {
      break;
    }
  }

  // 没有合适的大小来分配size
  if (bkt == NULL) {
    return NULL;
  }

  // 寻找还可以分配的page
  while (bkt != NULL && bkt->free == NULL) {
    bkt = bkt->next;
  }

  // 如果所有的bucket都已经分配，new一块新的bucket
  if (bkt == NULL) {
    bkt = new_bucket(bkt_size);
    bkt->next = bucket_dir[i];
    bucket_dir[i] = bkt;
  }

  bucket_block_t* p = bkt->free;
  bkt->free = p->next;
  return (void*)(p->addr);
}

void kfree(void* addr)
{
  bucket_desc_t* bkt = (void*)((unsigned int)addr & 0xFFFFF000);
  bucket_block_t* blk = (bucket_block_t*)addr;
  blk->next = bkt->free;
  bkt->free = blk;
}

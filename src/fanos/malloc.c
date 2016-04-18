#include <fanos/string.h>
#include <fanos/mem.h>
#include <fanos/type.h>

#define DESC_DIR_SIZE 8

typedef struct bucket_desc {
  // 这个bucket分配的内存块大小
  int size;

  // bucket已经分配出去的块个数
  int used;

  // bucket总共可以分配的块个数
  int cap;

  // 链表的next域，指向下一个同样size的bucket
  struct bucket_desc* next;

  // bucket可分配内存的首地址
  char* start;

  // 分配情况的bitmap
  unsigned char bitmap[32];
} bucket_desc_t;

// 16, 32...2048, 共8个类型
bucket_desc_t* bucket_dir[DESC_DIR_SIZE];

bucket_desc_t* new_bucket(int size)
{
  int cap = (4096 - sizeof(bucket_desc_t)) / size;
  bucket_desc_t* desc = get_free_page();
  memset(desc, 0, 4096);
  desc->start = (char*)desc + sizeof(bucket_desc_t);
  desc->size = size;
  desc->cap = cap;
  return desc;
}

void malloc_init()
{
  int i = 0;
  for (i=0; i<DESC_DIR_SIZE; i++) {
    bucket_dir[i] = new_bucket(16*(2<<i));
  }
}

void* malloc(int size)
{
  int i = 0;
  bucket_desc_t* bkt = NULL;
  for (i=0; i<DESC_DIR_SIZE; i++) {
    bkt = bucket_dir[i];
    if (size < bkt->size) {
      break;
    }
  }

  // 没有合适的大小来分配size
  if (bkt == NULL) {
    return NULL;
  }

  // 寻找还可以分配的page
  while (bkt != NULL && (bkt->used == bkt->cap)) {
    bkt = bkt->next;
  }

  // 如果所有的bucket都已经分配，new一块新的bucket
  if (bkt == NULL) {
    bucket_desc_t* bkt = new_bucket(bucket_dir[i]->size);
    bkt->next = bucket_dir[i];
    bucket_dir[i] = bkt;
  }

  // 在bucket内部寻找bitmap的块
  for (i=0; i<bkt->cap / 8; i++) {
    if (bkt->bitmap[i] != 0xFF) {
      break;
    }
  }

  // assert(i != bkt->cap / 8)
  // 找到块之后再找第几位
  unsigned char* p = bkt->bitmap + i;
  int j = 0;
  for (j=0; j<8; j++) {
    if ((*p & (1<<j)) == 0) {
      break;
    }
  }
  // 标记占用
  *p = *p | 1<<j;
  bkt->used++;
  return (void*)(bkt->start + size * (i*8+j));
}

void free(void* addr)
{
  bucket_desc_t* bkt = (void*)((unsigned int)addr & 0xFFFFF000);
  int offset = (unsigned int)addr - (unsigned int)(bkt->start);
  int i = offset / 8;
  int j = offset % 8;
  unsigned char* p = bkt->bitmap + i;
  *p = *p & ~(1<<j);
  bkt->used--;
}

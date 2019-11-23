#ifndef __MMU_HEAP_H
#define __MMU_HEAP_H

#include <stdint.h>

#define HEAP_WALKER(name) for(mblock_t *name = root_mblock; name != NULL; name = name->next) 

#define MBLOCK_MAGIC 0xDEAD 

struct mblock_struct {
  uint32_t  size;  
  uint16_t  magic;
  uint16_t  free;
  struct mblock_struct*  next;
};
typedef struct mblock_struct mblock_t;

void *malloc(uint32_t size);
void *calloc(size_t num, size_t size);
void free(void* size);
void debug_heap_dump();
void init_mmu_heap();

#endif

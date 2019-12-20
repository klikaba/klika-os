#include <klikaos.h>
#include <malloc.h>
#include <stdbool.h>
#include <string.h>

#define END_OF_INITIALISED_HEAP PAGE_SIZE

extern void* __user_app_end;

mblock_t *root_mblock;
uint64_t heap_start;
uint64_t heap_end;

uint32_t align_to(uint32_t addr, uint32_t align) {
  uint32_t align_mask = align - 1;
  if (addr & align_mask) {
    return (addr | align_mask) + 1;
  }
  else {
    return addr;
  }
}

mblock_t *find_heap_block(uint32_t size) {
  HEAP_WALKER(mb) {
    if (mb->free && mb->size > size) { // not >= ... little hack to not match last block in a byte 
      return mb;
    }
  }
  return NULL;
}

void debug_heap_dump() {
  DEBUG("Heap dump");
  HEAP_WALKER(mb) {
    DEBUG("Heap: mb: 0x%X mag:%X siz:%i free:%i", mb, mb->magic, mb->size, mb->free);
  }
}

void sbrk(uint32_t size) {
  DEBUG("Heap: Sbrk called : %i\n", size);

  // Ask more memory from kernel
  syscall(SYSCall_memory_sbrk);

  mblock_t *mb = root_mblock;
  while(mb->next != NULL) { mb = mb->next; }
  mb->size += PAGE_SIZE;
  heap_end += PAGE_SIZE;
  debug_heap_dump();
  DEBUG("Heap: Heap after srbk 0x%X\n", heap_end);
}


mblock_t *split_heap_block(mblock_t *mb, uint32_t size) {
  mb->free = false;
	// No splitting since new block can't fit
	if (mb->size < size + sizeof(mblock_t)) {
		return mb;
	}
  uint32_t old_size = mb->size;
  mblock_t *old_next = mb->next;
  uint8_t *ptr = (uint8_t*)mb;

  mb->size = size;

  mblock_t *next_mb = (mblock_t*)(ptr + sizeof(mblock_t) + mb->size);
  mb->next = next_mb;
  next_mb->magic = MBLOCK_MAGIC;
  next_mb->size = old_size - size - sizeof(mblock_t);
  next_mb->next = old_next;
  next_mb->free = true;

  return mb;
}

void *malloc(uint32_t size) {
  // Align next block to 64bits
  size = align_to(size, 8);
  mblock_t *mb = find_heap_block(size);
  if (mb == NULL) {
    sbrk(size);
    return malloc(size);
  } 
  else {
    mb = split_heap_block(mb, size);
  }
  return ((uint8_t*)mb) + sizeof(mblock_t);
}

void free(void *ptr) {
  mblock_t *mb = (mblock_t *)(((uint8_t*)ptr) - sizeof(mblock_t));
  if (mb->magic == MBLOCK_MAGIC) {
    mb->free = true;
  }
}

void *calloc(size_t num, size_t size) {
  void *ptr = malloc(num * size);
  memset(ptr, 0, num * size);
  return ptr;
}

void init_heap() {
  heap_start = (uint64_t)&__user_app_end;
  heap_end = END_OF_INITIALISED_HEAP;
  root_mblock = (mblock_t*)heap_start;
  root_mblock->free = true;
  root_mblock->magic = MBLOCK_MAGIC;
  root_mblock->size = heap_end - heap_start - sizeof(mblock_t);
  root_mblock->next = NULL;
}

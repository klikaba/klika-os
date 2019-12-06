#include <mmu_frames.h>
#include <mmu_paging.h>
#include <kernel.h>
#include <memory.h>
#include <stdint.h>
#include <string.h>
#include <x86.h>
#include <isr.h>

uint64_t mmu_nframes; // Number of entries in bitset
uint32_t *mmu_frames; // Start address of bitset

uint64_t heap_start;
uint64_t heap_end;

static uint64_t round_up(uint64_t num, uint64_t multiple) {
  return ((num + multiple - 1) / multiple) * multiple;
}

void mmu_frame_set(uint64_t frame_addr) {
  uint64_t frame = frame_addr / PAGE_SIZE;
  uint64_t idx = INDEX_FROM_BIT(frame);
  uint64_t off = OFFSET_FROM_BIT(frame);
  DEBUG("MMU[frames]: Frame set bit for 0x%X %i %i\n", frame_addr, idx, off);
  mmu_frames[idx] |= (0x1 << off);
}

// Static function to clear a bit in the frames bitset
void mmu_frame_clear(uint64_t frame_addr) {
  uint64_t frame = frame_addr / PAGE_SIZE;
  uint64_t idx = INDEX_FROM_BIT(frame);
  uint64_t off = OFFSET_FROM_BIT(frame);
  mmu_frames[idx] &= ~(0x1 << off);
}

// Static function to test if a bit is set.
uint64_t mmu_frame_test(uint64_t frame_addr) {
  uint64_t frame = frame_addr / PAGE_SIZE;
  uint64_t idx = INDEX_FROM_BIT(frame);
  uint64_t off = OFFSET_FROM_BIT(frame);
  return (mmu_frames[idx] & (0x1 << off));
}

uint64_t mmu_frame_find_first() {
  uint64_t i, j;
  for (i = 0; i < INDEX_FROM_BIT(mmu_nframes); i++)
  {
    if (mmu_frames[i] != 0xFFFFFFFF) // nothing free, exit early.
    {
      // at least one bit is free here.
      for (j = 0; j < 32; j++) {
        uint32_t toTest = 0x1 << j;
        if (!(mmu_frames[i] & toTest)) {
          return i*4*8+j;
        }
      }
    }
  }
  return 0;
}

uint64_t mmu_frame_count_used() {
  uint64_t i, j, count_used = 0;
  for (i = 0; i < INDEX_FROM_BIT(mmu_nframes); i++) {
    if (mmu_frames[i] == 0xFFFFFFFF) {
      count_used += 32;
    }
    else if (mmu_frames[i] != 0xFFFFFFFF) {
      // at least one bit is free here.
      for (j = 0; j < 32; j++) {
        uint32_t toTest = 0x1 << j;
        if (mmu_frames[i] & toTest) {
          count_used++;
        }
      }
    }
  }
  return count_used;
}

void mmu_frame_stats(mmu_frame_stats_t *stats_out) {
  stats_out->used_frames = mmu_frame_count_used();
  stats_out->free_frames = mmu_nframes - stats_out->used_frames;
}

void dump_frames() {
  DEBUG("Frames:\n");
  for (uint64_t i=0; i<INDEX_FROM_BIT(mmu_nframes); i++) {
    DEBUG("0x%X ", mmu_frames[i]);
  }
  DEBUG("\n");
}
  

void* alloc_frame() {
  uint64_t frame = mmu_frame_find_first(); 
  if (frame == 0) {
    HALT_AND_CATCH_FIRE("------------- NOT ENOUGH MEMORY!!!!! -------------");
    return NULL; // never reached
  }
  mmu_frame_set(frame * PAGE_SIZE);
  return (void*)(frame * PAGE_SIZE);
}

void free_frame(uint64_t ptr) {
  mmu_frame_clear(ptr);
}

void* alloc_frame_temp(uint64_t *phys_out) {
  uint64_t addr = (uint64_t)alloc_frame();
  DEBUG("MMU[frames]: alloc_frame_temp frame address 0x%X\n", addr);

  if (addr != 0) {
    *phys_out = addr;
    for (uint64_t i=0; i<512; i++) {
      if (pde_krnluser[i].all == 0) {
        DEBUG("MMU[frames]: alloc new frame %i\n", i);
        pde_krnluser[i].all = addr | PAGE_PRESENT_CPL0;

        x86_set_cr3(TO_PHYS_U64(pml4e));      
        x86_tlb_flush_all();

        return ((char*)KERNEL_USER_VIEW_START_MEMORY + PAGE_SIZE*i);
      }
    }
  }

  HALT_AND_CATCH_FIRE("NOT ENOUGH MEMORY TO MAP PROCESS SPACE");
  return NULL;
}

void init_mmu_frames(uint64_t memory_size) {
  mmu_nframes = memory_size / PAGE_SIZE;
  mmu_frames = (uint32_t*)&kernel_end;
  heap_start = round_up((uint64_t)(mmu_frames + mmu_nframes), 8);
  DEBUG("MMU[frames]: Frames %i, %i\n", mmu_nframes, INDEX_FROM_BIT(mmu_nframes));
  memset(mmu_frames, 0, INDEX_FROM_BIT(mmu_nframes) * sizeof(uint32_t));
  heap_end = round_up(heap_start, 0x200000); // Round up to end of page

  // MARK USED MEMORY
  // Dummy way : mark everything from 0x0000 - KERNEL_END (heap end)
  uint64_t kernel_end_p = TO_PHYS_U64(heap_end);
  uint64_t num_pages = kernel_end_p / PAGE_SIZE + 1;
  for (uint64_t i=0; i<num_pages; i++) {
    mmu_frame_set(i * PAGE_SIZE);
  }
  DEBUG("MMU[frames]: Used Mem %0iKB\n", TO_PHYS_U64(heap_end) / 1024 / 1024);
}
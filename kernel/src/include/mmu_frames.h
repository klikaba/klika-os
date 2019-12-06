#ifndef __MMU_FRAMES_H
#define __MMU_FRAMES_H

#include <stdint.h>

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

typedef struct {
  uint64_t used_frames;
  uint64_t free_frames;
} mmu_frame_stats_t;

extern uint64_t mmu_nframes; 
extern uint32_t *mmu_frames;

extern uint64_t heap_start;
extern uint64_t heap_end;

void mmu_frame_set(uint64_t frame_addr);
void mmu_frame_clear(uint64_t frame_addr);
uint64_t mmu_frame_test(uint64_t frame_addr);
uint64_t mmu_frame_find_first();
void dump_frames();
void* alloc_frame();
void free_frame(uint64_t ptr);
void mmu_frame_stats(mmu_frame_stats_t *stats_out);

void init_mmu_frames(uint64_t memory_size);

#endif
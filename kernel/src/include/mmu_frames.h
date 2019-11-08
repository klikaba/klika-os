#ifndef __MMU_FRAMES_H
#define __MMU_FRAMES_H

#include <stdint.h>

#define INDEX_FROM_BIT(a) (a/(8*4))
#define OFFSET_FROM_BIT(a) (a%(8*4))

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

void init_mmu_frames(uint64_t memory_size);

#endif
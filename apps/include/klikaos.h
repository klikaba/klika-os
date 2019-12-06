#ifndef __KLIKAOS_H
#define __KLIKAOS_H

#include <syscalls.h>

#define __UNUSED__ __attribute__((unused))
#define DEBUG _debug

#define PAGE_SIZE (2*1024*1024)

typedef struct {
  uint64_t used_frames;
  uint64_t free_frames;
} mmu_frame_stats_t;

void _debug(const char *fmt, ...);

void memory_stats(mmu_frame_stats_t *stats_out);

#endif
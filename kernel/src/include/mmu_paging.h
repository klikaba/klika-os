#ifndef __MMU_PAGING_H
#define __MMU_PAGING_H

#include <stdint.h>

extern void* kernel_end;

// 2MB Pages
#define PAGE_SIZE 0x200000

#define PAGE_PRESENT_CPL0 0x83
#define PAGE_PRESENT_CPL3 0x87

typedef union pml4e_struct {
  uint64_t all;
  struct {
    uint64_t present         : 1;   // Page present in memory
    uint64_t writable        : 1;   // Read-only if clear, readwrite if set
    uint64_t user            : 1;   // Supervisor level only if clear
    uint64_t write_through   : 1;   // Write through flag
    uint64_t disable_cache   : 1;   // Disable cache 
    uint64_t accessed        : 1;   // Has the page been accessed since last refresh?
    uint64_t ignore_1        : 1;   // Ignored
    uint64_t mbz_1           : 1;   // must be zero
    uint64_t mbz_2           : 1;   // must be zero
    uint64_t avail_1         : 3;   // Available for OS to use
    uint64_t address         : 40;  // Address
    uint64_t avail_2         : 11;  // Available for OS to use
    uint64_t no_exec         : 1;   // Frame address (shifted right 12 bits)
  } fields;
} __attribute__((packed)) pml4e_t;

typedef union pdpe_struct {
  uint64_t all;
  struct {
    uint64_t present         : 1;   // Page present in memory
    uint64_t writable        : 1;   // Read-only if clear, readwrite if set
    uint64_t user            : 1;   // Supervisor level only if clear
    uint64_t write_through   : 1;   // Write through flag
    uint64_t disable_cache   : 1;   // Disable cache 
    uint64_t accessed        : 1;   // Has the page been accessed since last refresh?
    uint64_t ignore_1        : 1;   // Ignored
    uint64_t mbz_1           : 1;   // must be zero
    uint64_t ignore_2        : 1;   // Ignored
    uint64_t avail_1         : 3;   // Available for OS to use
    uint64_t address         : 40;  // Address
    uint64_t avail_2         : 11;  // Available for OS to use
    uint64_t no_exec         : 1;   // Frame address (shifted right 12 bits)
  } fields;
} __attribute__((packed)) pdpe_t;

typedef union pde_struct {
  uint64_t all;
  struct {
    uint64_t present         : 1;   // Page present in memory
    uint64_t writable        : 1;   // Read-only if clear, readwrite if set
    uint64_t user            : 1;   // Supervisor level only if clear
    uint64_t write_through   : 1;   // Write through flag
    uint64_t disable_cache   : 1;   // Disable cache 
    uint64_t accessed        : 1;   // Has the page been accessed since last refresh?
    uint64_t dirty           : 1;   // Ignored
    uint64_t mbo_1           : 1;   // Must be one
    uint64_t global          : 1;   // Global
    uint64_t avail_1         : 3;   // Available for OS to use
    uint64_t pat             : 1;   // PAT translation mode
    uint64_t mbz_1           : 8;   // Must be zero
    uint64_t address         : 31;  // Address
    uint64_t avail_2         : 11;  // Available for OS to use
    uint64_t no_exec         : 1;   // Frame address (shifted right 12 bits)
  } fields;
} __attribute__((packed)) pde_t;

extern pml4e_t pml4e[512];
extern pdpe_t  pdpe[512];
extern pde_t   pde[512];
extern pdpe_t  pdpe_user[512];
extern pde_t   pde_user[512];
extern pdpe_t  pdpe_krnluser[512];
extern pdpe_t  pde_krnluser[512];

void* alloc_frame_temp(uint64_t *phys_out);
void init_kernel_paging();

#endif
#include <mmu_pagging.h>
#include <mmu_frames.h>
#include <mmu_heap.h>
#include <kernel.h>
#include <memory.h>
#include <stdint.h>
#include <string.h>
#include <x86.h>
#include <isr.h>

pml4e_t pml4e[512] __attribute__((aligned(4096)));
pdpe_t  pdpe[512] __attribute__((aligned(4096)));
pde_t   pde[512] __attribute__((aligned(4096)));

pdpe_t  pdpe_user[512] __attribute__((aligned(4096)));
pde_t   pde_user[512] __attribute__((aligned(4096)));

pdpe_t  pdpe_krnluser[512] __attribute__((aligned(4096)));
pdpe_t  pde_krnluser[512] __attribute__((aligned(4096)));

static void init_pagging_kernel() {
  // Setup new PAGGING DIRECTORY
  memset(&pml4e, 0, sizeof(pml4e_t) * 512);
  memset(&pdpe, 0, sizeof(pdpe_t) * 512);
  memset(&pde, 0, sizeof(pde_t) * 512);
  memset(&pdpe_krnluser, 0, sizeof(pdpe_t) * 512);
  memset(&pde_krnluser, 0, sizeof(pde_t) * 512);

  // Kernel space
  pml4e[0x100].all = TO_PHYS_U64(&pdpe) | 3; // KERNEL_VMA: Present + Write
  pml4e[0x1C0].all = TO_PHYS_U64(&pdpe_krnluser) | 3; // KERNEL_USER TO KRNLVMA: Present + Write
  pdpe[0].all = TO_PHYS_U64(&pde) | 3; 
  pdpe_krnluser[0].all = TO_PHYS_U64(&pde_krnluser) | 3; 

  uint64_t num_pages = TO_PHYS_U64(heap_end) / PAGE_SIZE;
  DEBUG("MMU[pagging]: Krnl space pages needed: %i\n\r", num_pages);

  for (uint64_t i=0; i<=num_pages; i++) {
    pde[i].all = (i * PAGE_SIZE) | 0x83; // Presetn + Write + Large (2MB)
  }
}

static void init_pagging_user() {
  // User space
  memset(&pdpe_user, 0, sizeof(pdpe_t) * 512);
  memset(&pde_user, 0, sizeof(pde_t) * 512);

  pml4e[0].all = TO_PHYS_U64(&pdpe_user) | 0x7; // USER_VMA: Present + Write + CPL3
  pdpe_user[0].all = TO_PHYS_U64(&pde_user) | 0x07; 
}

void init_kernel_pagging() {
  DEBUG("MMU[pagging] PAGGING: init ...");
  init_mmu_frames(128 * 1024 * 1024); // 128Megs for now - TODO : autodetect);
  init_pagging_kernel();
  init_pagging_user();
  x86_set_cr3(TO_PHYS_U64(pml4e));
  init_mmu_heap();
}

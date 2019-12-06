#include <mmu_paging.h>
#include <mmu_frames.h>
#include <process.h>
#include <kernel.h>
#include <string.h>
#include <isr.h>
#include <x86.h>

#define __UNUSED__ __attribute__((unused))

// syscall_memory_sbrk()
long syscall_memory_sbrk(isr_ctx_t *regs __UNUSED__) {
	DEBUG("SYSCALL[memory]: Sbrk called for task %i\n", task_list_current->id);

	uint64_t phys_addr = (uint64_t)alloc_frame(&phys_addr);

	for (int i=0; i<512; i++) {
		if (task_list_current->pde[i].all == 0) {
			// Allocate nw PDE entry in task structure 
			task_list_current->pde[i].all = phys_addr | PAGE_PRESENT_CPL3; // Present + Write + CPL3
			// Refresh PDE table
		  memcpy(pde_user, task_list_current->pde, 512 * sizeof(pde_t));
			x86_tlb_flush_all();
			return 1;
		}
	}
	return 0;
}

// syscall_memory_stats(mmu_frame_stats_t *stats_out)
long syscall_memory_stats(isr_ctx_t *regs) {
	mmu_frame_stats_t *stats_out = (mmu_frame_stats_t*)regs->rdi;
	mmu_frame_stats(stats_out);
	return 0;
}
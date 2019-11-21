#include <window_manager.h>
#include <mmu_heap.h>
#include <process.h>
#include <kernel.h>
#include <string.h>
#include <vesa.h>
#include <gfx.h>
#include <isr.h>

// syscall_process_create(void* elf_data)
uint64_t syscall_process_create(isr_ctx_t *regs) {
	void *elf_data = (void*)regs->rdi;
	task_t *task = create_user_process(elf_data);
	
	return task != NULL ? task->id : 0;
}


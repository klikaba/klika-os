#include <window_manager.h>
#include <mmu_heap.h>
#include <process.h>
#include <kernel.h>
#include <string.h>
#include <vesa.h>
#include <gfx.h>
#include <isr.h>

// syscall_windows_create(int x, int y, int width, int height, char* title)
uint64_t syscall_windows_create(isr_ctx_t *regs) {
	int x = regs->rdi;
	int y = regs->rsi;
	int width = regs->rdx;
	int height = regs->rcx;
	uint32_t attributes = (uint32_t) regs->r8;
	window_t* win = window_create(x, y, width, height, attributes);
	
	return win->handle;
}

// syscall_windows_present(uint32_t handle, context_t*)
uint64_t syscall_windows_present(isr_ctx_t *regs) {
	window_t* win = window_find(regs->rdi);
	if (win != NULL) {
		context_t* context = (context_t*)regs->rsi;
		window_present_context(win, context);
		return 1;
	}
	return 0;
}

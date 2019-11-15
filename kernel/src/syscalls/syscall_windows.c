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
	char* title = (char*)regs->r8;
	window_t* win = window_create(x, y, width, height, title);
	
	return win->handle;
}

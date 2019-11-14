#include <window_manager.h>
#include <mmu_heap.h>
#include <kernel.h>
#include <string.h>
#include <vesa.h>
#include <gfx.h>
#include <isr.h>

//RDI, RSI, RDX, RCX, R8, R9
uint64_t syscall_windows_create(isr_ctx_t *regs) {
	int x = regs->rdi;
	int y = regs->rsi;
	int width = regs->rdx;
	int height = regs->rcx;
	window_t* win = window_create(x, y, width, height);
	
	return win->handle;
}

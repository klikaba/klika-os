#include <window_manager.h>
#include <mmu_heap.h>
#include <process.h>
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
	char* title = (char*)to_kernel_space(task_list_current, regs->r8);
	window_t* win = window_create(x, y, width, height, title);
	
	return win->handle;
}

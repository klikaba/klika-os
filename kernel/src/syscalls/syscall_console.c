#include <kernel.h>
#include <console.h>
#include <isr.h>

extern console_t __krnl_console;

#define __UNUSED__ __attribute__((unused))

long sysc_console_putc(isr_ctx_t *regs) {
  kprintf_xy(regs->rdi, regs->rsi, "%i", regs->rdx);
	return 0;
}

long sysc_console_clear(isr_ctx_t *regs __UNUSED__) {
  console_clear(&__krnl_console);
	return 0;  
}



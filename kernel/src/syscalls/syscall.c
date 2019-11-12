#include <kernel.h>
#include <isr.h>

typedef long (*sys_call_ptr_t)(isr_ctx_t *regs);

#define __SYSCALL(nr, sym) extern long sym(isr_ctx_t *regs);
#include <syscalls.h>
#undef __SYSCALL

#define __SYSCALL(nr, sym) [nr] = sym,
const sys_call_ptr_t sys_call_table[] = {
	#include <syscalls.h>
};
#undef __SYSCALL

#define dim(x) (sizeof(x) / sizeof((x)[0]))

long system_call_handler(isr_ctx_t *regs) {
  kprintf_xy(10, 3, "SYSCALL: %i", regs->rax);

  uint64_t idx = regs->rax;
  if (idx < dim(sys_call_table)) {
  	return (sys_call_table[idx])(regs);
  }
	return -1;
}


#include <process.h>
#include <kernel.h>
#include <stdint.h>
#include <isr.h>

typedef uint64_t (*sys_call_ptr_t)(isr_ctx_t *regs);

#define __SYSCALL(nr, sym) extern uint64_t sym(isr_ctx_t *regs);
#include <syscalls.h>
#undef __SYSCALL

#define __SYSCALL(nr, sym) [nr] = sym,
const sys_call_ptr_t sys_call_table[] = {
	#include <syscalls.h>
};
#undef __SYSCALL

#define dim(x) (sizeof(x) / sizeof((x)[0]))

//RDI, RSI, RDX, RCX, R8, R9
uint64_t system_call_handler(isr_ctx_t *regs) {
  // DEBUG("SYSCALL[%i]:  %i(%i %i %i %i %i %i)\n", task_list_current->id, regs->rax, regs->rdi, 
  // 			regs->rsi, regs->rdx, regs->rcx, regs->r8, regs->r9);

  uint64_t idx = regs->rax;
  if (idx < dim(sys_call_table)) {
  	return (sys_call_table[idx])(regs);
  }
  DEBUG("SYSCALL: %i not found!\n", regs->rax);
	return 0xFFFFFFFFFFFFFFFF;
}


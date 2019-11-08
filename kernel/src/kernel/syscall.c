#include <kernel.h>
#include <isr.h>

void system_call_handler(isr_ctx_t *regs) {
    kprintf_xy(10, 0, "SYSCALL: 0x%X", regs->rax);
}


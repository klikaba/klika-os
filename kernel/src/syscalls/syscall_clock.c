#include <kernel.h>
#include <clock.h>
#include <isr.h>

// syscall_clock_get_time()
uint64_t syscall_get_clock(isr_ctx_t *regs __UNUSED__) 
{
	return get_hw_datetime();
}

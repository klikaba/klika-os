#include <kernel.h>
#include <clock.h>
#include <isr.h>

uint64_t syscall_get_clock(isr_ctx_t *regs __UNUSED__)
{
	return get_hw_datetime();
}

uint64_t syscall_get_clock_ticks(isr_ctx_t *regs __UNUSED__)
{
	return get_rtc_ticks();
}
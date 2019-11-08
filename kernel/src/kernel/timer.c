#include <timer.h>
#include <isr.h>
#include <memory.h>
#include <console.h>
#include <pic.h>
#include <x86.h>

uint64_t __tick = 0;

void timer_callback(isr_ctx_t *ctx __attribute__((unused))) {
    __tick++;

    // Change color - top-right char
    *(CONSOLE_VIDEO_MEMORY+159) += 1;
}

uint64_t timer_tick() {
    return __tick;
}

void init_kernel_timer() {
    // Setup 50Hz timer
    int divisor = 1193180 / TIMER_HZ;
    outp(0x43, 0x36);
    outp(0x40, divisor & 0xff);
    outp(0x40, divisor >> 8);

    // Start timer IRQ
    irq_enable(PIC_IRQ0);
}

void krnl_delay(unsigned int delay) {
    uint64_t start_pit, end_pit, gap;

    start_pit = __tick;
    gap = delay / (1000 / TIMER_HZ);
    end_pit = start_pit + gap;

    while (__tick < end_pit);
}

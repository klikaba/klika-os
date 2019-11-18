#include <window_manager.h>
#include <timer.h>
#include <memory.h>
#include <isr.h>
#include <pic.h>
#include <x86.h>

volatile uint64_t __tick = 0;

void timer_callback() {
    __tick++;
    // win manager will decide does it need to redraw or not
    window_manager_redraw();
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

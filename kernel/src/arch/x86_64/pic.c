#include <pic.h>
#include <x86.h>

void init_kernel_pic() {
    // ICW1
    outp(PIC_MASTER_CTRL, 0x11);  // init master PIC
    outp(PIC_SLAVE_CTRL, 0x11);   // init slave PIC
    // ICW2
    outp(PIC_MASTER_DATA, 0x20);  // IRQ 0..7 remaped to 0x20..0x27
    outp(PIC_SLAVE_DATA, 0x28);   // IRQ 8..15 remaped to 0x28..0x37
    // ICW3
    outp(PIC_MASTER_DATA, 0x04);  // set as Master
    outp(PIC_SLAVE_DATA, 0x02);   // set as Slave
    // ICW4
    outp(PIC_MASTER_DATA, 0x01);  // set x86 mode
    outp(PIC_SLAVE_DATA, 0x01);   // set x86 mode

    outp(PIC_MASTER_DATA, 0xff);  // all interrupts disabled
    outp(PIC_SLAVE_DATA, 0xff);

    // JUST IN CASE ?!?
    __asm__ __volatile__("nop");
}

static uint16_t ocw1 = 0xFFFB;

void irq_enable(uint8_t irq) {
    ocw1 &= (uint16_t)~((1 << irq));
    if (irq < 8) {
        outp(PIC_MASTER_DATA, (uint8_t)(ocw1 & 0xFF));
    }
    else {
        outp(PIC_SLAVE_DATA, (uint8_t)(ocw1 >> 8));
    }
}


void pic_acknowledge(uint8_t irq) {
    if (irq > 7) {
        outp(PIC_SLAVE_CTRL, 0x20);
    }
    outp(PIC_MASTER_CTRL, 0x20);
}

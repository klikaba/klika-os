#ifndef __PIC_H_
#define __PIC_H_

#include <stdint.h>

#define IRQ_BASE  0x20

#define PIC_MASTER_CTRL 0x20
#define PIC_MASTER_DATA 0x21
#define PIC_SLAVE_CTRL  0xA0
#define PIC_SLAVE_DATA  0xA1

// IRQ 0..7 remaped to 0x20..0x27 
// IRQ 8..15 remaped to 0x28..0x37
#define PIC_IRQ0 0x00
#define PIC_IRQ1 0x01
#define PIC_IRQ2 0x02
#define PIC_IRQ3 0x03
#define PIC_IRQ4 0x04
#define PIC_IRQ5 0x05
#define PIC_IRQ6 0x06
#define PIC_IRQ7 0x07
#define PIC_IRQ8 0x08
#define PIC_IRQ9 0x09
#define PIC_IRQA 0x0A
#define PIC_IRQB 0x0B
#define PIC_IRQC 0x0C
#define PIC_IRQD 0x0D
#define PIC_IRQE 0x0E
#define PIC_IRQF 0x0F

void init_kernel_pic();
void pic_acknowledge(uint8_t irq);
void irq_enable(uint8_t irq);

#endif

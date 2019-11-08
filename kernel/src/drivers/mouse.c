#include <mouse.h>
#include <string.h>
#include <kernel.h>
#include <x86.h>
#include <isr.h>
#include <pic.h>

void mouse_wait(uint8_t a_type) {
  uint32_t _time_out=100000; 
  if(a_type == 0) {
    while(_time_out--) //Data
    {
      if((inp(0x64) & 1) == 1) {
        return;
      }
    }
    return;
  }
  else {
    while(_time_out--) //Signal
      {
      if((inp(0x64) & 2) == 0) {
        return;
      }
    }
    return;
  }
}


void mouse_write(uint8_t write) {
  mouse_wait(1);
  outp(0x64, 0xD4);
  mouse_wait(1);
  outp(0x60, write);
}

uint8_t mouse_read() {
  mouse_wait(0);
  return inp(0x60);
}

mouse_packet_t mouse_packet;

static int8_t mouse_byte[3];
static int8_t mouse_x=0;
static int8_t mouse_y=0;
static uint8_t mouse_cycle=0;

static void mouse_callback(isr_ctx_t *ctx __attribute__((unused))) {
  switch(mouse_cycle)
  {
    case 0:
      mouse_byte[0] = inp_s(0x60);
      mouse_cycle++;
      break;
    case 1:
      mouse_byte[1] = inp_s(0x60);
      mouse_cycle++;
      break;
    case 2:
      mouse_byte[2] = inp_s(0x60);
      mouse_x = mouse_byte[1];
      mouse_y = mouse_byte[2];
      mouse_cycle = 0;
      break;
  }
  if (mouse_cycle == 0) {
    kprintf_xy(30, 0, "M: (%i,%i)        ", mouse_x, mouse_y);
  }
  pic_acknowledge(PIC_IRQC);
}


void init_kernel_mouse() {
  uint8_t status;

  // queue_init(&mouse_queue, 32);
  // queue_init(&mouse_wait, 8);
  mouse_wait(1);
  outp(0x64, 0xA8);

  //Enable the interrupts
  mouse_wait(1);
  outp(0x64, 0x20);

  mouse_wait(0);
  status = (inp(0x60) | 2);

  mouse_wait(1);
  outp(0x64, 0x60);

  mouse_wait(1);
  outp(0x60, status);

  // Tell the mouse to use default settings
  mouse_write(0xF6);
  mouse_read();  // ACK

  // Enable the mouse
  mouse_write(0xF4);
  mouse_read();  // ACK

  register_interrupt_handler(ISR_IRQC, mouse_callback);
  irq_enable(PIC_IRQC);
}

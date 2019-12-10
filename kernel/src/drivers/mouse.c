#include <window_manager.h>
#include <mouse.h>
#include <string.h>
#include <kernel.h>
#include <gfx.h>
#include <x86.h>
#include <isr.h>
#include <pic.h>

static uint8_t mouse_cycle = 0;
static uint8_t mouse_byte[4];
int32_t mouse_x = 0;
int32_t mouse_y = 0;
int32_t mouse_x_difference = 0;
int32_t mouse_y_difference = 0;
int32_t mouse_buttons;

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

message_t old_message;

static void mouse_callback(isr_ctx_t *ctx __attribute__((unused))) {
  uint8_t status = inp(MOUSE_STATUS);
  while ((status & MOUSE_BBIT) && (status & MOUSE_F_BIT)) {
    int8_t mouse_in = inp(MOUSE_PORT);
    switch (mouse_cycle) {
      case 0:
        mouse_byte[0] = mouse_in;
        if (!(mouse_in & MOUSE_V_BIT)) break;
        ++mouse_cycle;
        break;
      case 1:
        mouse_byte[1] = mouse_in;
        ++mouse_cycle;
        break;
      case 2:
        mouse_byte[2] = mouse_in;
        // if (mouse_mode == MOUSE_SCROLLWHEEL || mouse_mode == MOUSE_BUTTONS) {
        //   ++mouse_cycle;
        //   break;
        // }
        goto finish_packet;
      case 3:
        mouse_byte[3] = mouse_in;
        goto finish_packet;
    }
    goto read_next;
finish_packet:
    mouse_cycle = 0;
    /* We now have a full mouse packet ready to use */
    message_t message;
    message.message = MESSAGE_MOUSE_MOVE;
    int x = mouse_byte[1];
    int y = mouse_byte[2];
    if (x && mouse_byte[0] & (1 << 4)) {
      /* Sign bit */
      x = x - 0x100;
    }
    if (y && mouse_byte[0] & (1 << 5)) {
      /* Sign bit */
      y = y - 0x100;
    }
    if (mouse_byte[0] & (1 << 6) || mouse_byte[0] & (1 << 7)) {
      /* Overflow */
      x = 0;
      y = 0;
    }
    mouse_x_difference = x;
    mouse_y_difference = y;
    message.key = 0;
    if (mouse_byte[0] & 0x01) {
      message.message = MESSAGE_MOUSE_PRESS;
      message.key |= MOUSE_LEFT_CLICK;
    }
    if (mouse_byte[0] & 0x02) {
      message.message = MESSAGE_MOUSE_PRESS;
      message.key |= MOUSE_RIGHT_CLICK;
    }
    if (mouse_byte[0] & 0x04) {
      message.message = MESSAGE_MOUSE_PRESS;
      message.key |= MOUSE_MIDDLE_CLICK;
    }

    // TODO - remove global once 
    mouse_buttons = message.key;
    mouse_x += mouse_x_difference;
    mouse_y -= mouse_y_difference;
    if (mouse_x > 1024) mouse_x = 1024;
    if (mouse_x < 0) mouse_x = 0;
    if (mouse_y > 768) mouse_y = 768;
    if (mouse_y < 0) mouse_y = 0;
    message.x = mouse_x;
    message.y = mouse_y;

    // Do not send same message twice
    if (old_message.message != message.message || old_message.x != message.x ||
        old_message.y != message.y || old_message.key != message.key ) {
      if (old_message.message == MESSAGE_MOUSE_PRESS) {
        message.message = MESSAGE_MOUSE_RELEASE;
      }
      window_handle_mouse();
      window_add_message_to_focused(&message);
      old_message = message;
    }
read_next:
    break;
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

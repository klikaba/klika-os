#ifndef __MOUSE_H
#define __MOUSE_H

#include <stdint.h>

#define PACKETS_IN_PIPE 1024
#define DISCARD_POINT 32

#define MOUSE_IRQ 12

#define MOUSE_PORT   0x60
#define MOUSE_STATUS 0x64
#define MOUSE_ABIT   0x02
#define MOUSE_BBIT   0x01
#define MOUSE_WRITE  0xD4
#define MOUSE_F_BIT  0x20
#define MOUSE_V_BIT  0x08

#define MOUSE_DEFAULT 0
#define MOUSE_SCROLLWHEEL 1
#define MOUSE_BUTTONS 2

#define MOUSE_LEFT_CLICK   		0x01
#define MOUSE_RIGHT_CLICK  		0x02
#define MOUSE_MIDDLE_CLICK 		0x04
#define MOUSE_MOUSE_SCROLL_UP 0x10
#define MOUSE_MOUSE_SCROLL_DOWN 0x20

extern int32_t mouse_x;
extern int32_t mouse_y;
extern int32_t mouse_x_difference;
extern int32_t mouse_y_difference;
extern int32_t mouse_buttons;

void init_kernel_mouse(void);

#endif
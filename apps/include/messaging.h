#ifndef __MESSAGING_H
#define __MESSAGING_H

#include <stdint.h>

#define MESSAGE_MOUSE_MOVE 		1
#define MESSAGE_MOUSE_CLICK 	2
#define MESSAGE_KEY_PRESS		  3
#define MESSAGE_KEY_RELEASE   4

typedef enum {
	LEFT_CLICK   = 0x01,
	RIGHT_CLICK  = 0x02,
	MIDDLE_CLICK = 0x04,

	MOUSE_SCROLL_UP = 0x10,
	MOUSE_SCROLL_DOWN = 0x20,
} mouse_click_t;

typedef struct {
	uint32_t 	message;
	uint32_t 	handle;
	int16_t 	mouse_x;
	int16_t 	mouse_y;
	mouse_click_t 	mouse_buttons;
	uint16_t 	key;
} message_t;

#endif
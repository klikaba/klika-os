#ifndef __MESSAGING_H
#define __MESSAGING_H

#include <mouse.h>

#define MESSAGE_MOUSE_MOVE 		1
#define MESSAGE_MOUSE_CLICK 	2
#define MESSAGE_KEY_PRESS 	  3
#define MESSAGE_KEY_RELEASE	  4

typedef struct {
	uint32_t 	message;
	uint32_t 	handle;
	int16_t 	mouse_x;
	int16_t 	mouse_y;
	mouse_click_t 	mouse_buttons;
	uint16_t 	key;
	uint64_t  param0;
} message_t;

#endif
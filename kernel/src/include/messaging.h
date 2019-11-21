#ifndef __MESSAGING_H
#define __MESSAGING_H

#include <mouse.h>

#define MESSAGE_MOUSE_MOVE 		1
#define MESSAGE_MOUSE_PRESS 	2
#define MESSAGE_MOUSE_RELEASE	3
#define MESSAGE_KEY_PRESS		  4
#define MESSAGE_KEY_RELEASE   5
#define MESSAGE_WINDOW_DRAG	  6

typedef struct {
	uint16_t 		message;
	uint16_t 		handle;
	int16_t 		x;
	int16_t 		y;
	int32_t 		key;
} message_t;

#endif
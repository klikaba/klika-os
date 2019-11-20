#ifndef __MESSAGING_H
#define __MESSAGING_H

#include <windows/window.h>
#include <stdint.h>

#define MESSAGE_MOUSE_MOVE 		1
#define MESSAGE_MOUSE_CLICK 	2
#define MESSAGE_KEY_PRESS		  3
#define MESSAGE_KEY_RELEASE   4
#define MESSAGE_WINDOW_DRAG	  5

#define MESSAGE_MOUSE_MOVE 		1
#define MESSAGE_MOUSE_CLICK 	2
#define MESSAGE_KEY_PRESS 	  3
#define MESSAGE_KEY_RELEASE	  4
#define MESSAGE_WINDOW_DRAG	  5

typedef struct message_struct {
	uint16_t 		message;
	uint16_t 		handle;
	int16_t 		x;
	int16_t 		y;
	int32_t 		key;
} message_t;

void window_send_message(window_t* win, message_t* msg);
void window_send_message_simple(window_t* win, int message);
bool window_get_message(window_t* win, message_t* msg);

#endif
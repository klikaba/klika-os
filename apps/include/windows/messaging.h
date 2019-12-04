#ifndef __MESSAGING_H
#define __MESSAGING_H

#include <windows/window.h>
#include <stdint.h>

#define MESSAGE_MOUSE_MOVE 		1
#define MESSAGE_MOUSE_PRESS 	2
#define MESSAGE_MOUSE_RELEASE	3
#define MESSAGE_KEY_PRESS		  4
#define MESSAGE_KEY_RELEASE   5
#define MESSAGE_WINDOW_DRAG	  6

#define MOUSE_LEFT_CLICK   			0x01
#define MOUSE_RIGHT_CLICK  			0x02
#define MOUSE_MIDDLE_CLICK 			0x04
#define MOUSE_MOUSE_SCROLL_UP 	0x10
#define MOUSE_MOUSE_SCROLL_DOWN 0x20

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
bool window_peek_message(window_t* win, message_t* msg);

#endif
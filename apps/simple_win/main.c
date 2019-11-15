#include <messaging.h>
#include <klika_os.h>
#include <syscalls.h>
#include <stdbool.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef uint32_t window_handle;

#define WINDOW_STATE_CREATED 0
#define WINDOW_STATE_DESTORY 1

#define WINDOW_LIB_MESSAGE_CREATE 10000
#define WINDOW_LIB_MESSAGE_DRAW   10001

struct window_struct;
typedef bool (*window_procedure_t)(message_t *, struct window_struct *);

struct window_struct {
	window_handle handle;
	window_procedure_t window_procedure;
	window_procedure_t default_window_procedure;
	bool redraw;
	int state;
};
typedef struct window_struct window_t;

/////////////////// MESSAGING /////////////////////////

void window_send_message(message_t* msg, window_t* win) {
	syscall(SYSCall_messaging_create, msg, win->handle);
}

void window_send_message_simple(int message, window_t* win) {
	message_t msg;
	msg.message = message;
	window_send_message(&msg, win);
}

bool window_get_message(message_t* msg, window_t* win) {
	return syscall(SYSCall_messaging_get, msg, win->handle);
}

/////////////////// WINDOW PROCEDURE /////////////////////////

void window_dispatch_message(message_t* msg, window_t* win) {
	bool consumed = false;
	if (win->window_procedure != NULL) {
		consumed = win->window_procedure(msg, win);
	}
	if (!consumed) {
		win->default_window_procedure(msg, win);
	}
}

bool window_procedure(message_t* msg, struct window_struct* win) {
	switch(msg->message) {
		// case PAINT:
		// case CREATE:
		// case DESTROY:
		default:
			return win->default_window_procedure(msg, win);
	}
}

bool default_window_procedure(message_t* msg, struct window_struct* win) {
	DEBUG("Default win proc: win[%i] <- %i", win->handle, msg->message);
	switch(msg->message) {
		// case PAINT:
		case WINDOW_LIB_MESSAGE_CREATE:
			window_send_message_simple(WINDOW_LIB_MESSAGE_DRAW, win);
			break;		
		// case DESTROY:
	}
	return true;
}

///////////////////// WINDOW ACTIONS ///////////////////////

void window_change_state(window_t* win, int state) {
	switch(state) {
		case WINDOW_STATE_CREATED:
			win->state = state;
			window_send_message_simple(WINDOW_LIB_MESSAGE_CREATE, win);
			break;
	}
}

window_t window_create(int x, int y, int width, int height, char* title) {
	window_t window;
	memset(&window, 0, sizeof(window_t));
	window.window_procedure = &window_procedure;
	window.default_window_procedure = &default_window_procedure;
	window.handle = syscall(SYSCall_windows_create, x, y, width, height, title);
	window_change_state(&window, WINDOW_STATE_CREATED);

	return window;
}


/////////////////////////// GRAPHICS CONTEXT ////////////////

message_t msg;
window_t window;

int main() {
	window = window_create(100, 100, 600, 400, "Hello World!");

	while(window_get_message(&msg, &window)) { 
		if (msg.message != 0) {
			switch(msg.message) {
				case MESSAGE_MOUSE_MOVE:
					DEBUG("Message: %i - Mouse move (%i, %i)", msg.message, msg.mouse_x, msg.mouse_y);
					break;
				case MESSAGE_MOUSE_CLICK:
					DEBUG("Message: %i - Mouse click (%i, %i) : %i", msg.message, msg.mouse_x, msg.mouse_y, msg.mouse_buttons);
					break;
				case MESSAGE_KEY_PRESS:
					DEBUG("Message: %i - Key press (%c)", msg.message, msg.key & 0xFF);
					break;
				case MESSAGE_KEY_RELEASE:
					DEBUG("Message: %i - Key release (%c)", msg.message, msg.key & 0xFF);
					break;
				default:
					DEBUG("Got message : %i", msg.message);
					break;
			}
			window_dispatch_message(&msg, &window);
		}
	}
	return 0;
}
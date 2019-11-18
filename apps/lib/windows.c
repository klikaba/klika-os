#include <windows.h>
#include <messaging.h>
#include <klika_os.h>
#include <syscalls.h>
#include <stdbool.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <gfx.h>

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
	return win->default_window_procedure(msg, win);
}

bool default_window_procedure(message_t* msg, struct window_struct* win) {
	// DEBUG("Default win proc: win[%i] <- %i", win->handle, msg->message);
	switch(msg->message) {
		case WINDOW_LIB_MESSAGE_CREATE:
			window_send_message_simple(WINDOW_LIB_MESSAGE_PREDRAW, win);
			break;		
		case WINDOW_LIB_MESSAGE_PREDRAW:
			window_predraw(win);
			window_send_message_simple(WINDOW_LIB_MESSAGE_DRAW, win);
			return true;
		case WINDOW_LIB_MESSAGE_DRAW:
			window_send_message_simple(WINDOW_LIB_MESSAGE_PRESENT, win);
			return true;
		case WINDOW_LIB_MESSAGE_PRESENT:
			syscall(SYSCall_windows_present, win->handle, &win->context);
			return true;
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

static void context_create(window_t* win, int width, int height, int bpp) {
	int context_size = width * height * (bpp / 8);
	win->context.width = width;
	win->context.height = height;
	win->context.bpp = bpp;
	win->context.buffer = (uint32_t*)malloc(context_size);
	memset(win->context.buffer, 0xFF, context_size);
}

window_t window_create(int x, int y, int width, int height, char* title) {
	window_t window;
	memset(&window, 0, sizeof(window_t));
	window.window_procedure = &window_procedure;
	window.default_window_procedure = &default_window_procedure;
	window.redraw = true;
	context_create(&window, width, height, 32);
	window.handle = syscall(SYSCall_windows_create, x, y, width, height, title);
	window_change_state(&window, WINDOW_STATE_CREATED);
	return window;
}

void window_predraw(window_t *win) {
	int x1 = 0;
	int y1 = 0;
	int x2 = win->context.width;
	int y2 = win->context.height;

	// Main background
	gfx_rect(&win->context, x1, y1, x2, y2, WIN_BACKGROUND_COLOR);
	// Draw frame border
	gfx_rect(&win->context, x1, x2, y1, y2, WIN_FRAME_COLOR);

	// TODO syscall - syscall_windows_is_top(win->handle)
	bool is_top = false;
	uint32_t top_frame_color = is_top ? WIN_ACTIVE_FRAME : WIN_INACTIVE_FRAME;
	gfx_fillrect(&win->context, x1 + 1, y1 + 1, x2 - 1, y1 + WINDOW_BAR_HEIGHT, top_frame_color);

	// Frame label
	gfx_puts(&win->context, x1 + 1 + 3, y1 + 1 + 3, WIN_FRAME_TEXT_COLOR, top_frame_color, "Hello world");
}

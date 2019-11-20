#include <windows.h>
#include <windows/messaging.h>
#include <klika_os.h>
#include <syscalls.h>
#include <stdbool.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <gfx.h>

bool window_default_procedure(window_t *win, message_t *msg) {
	// DEBUG("Default win proc: win[%i] <- %i", win->handle, msg->message);
	switch(msg->message) {
		case MESSAGE_WINDOW_DRAG:
			win->x = msg->x;
			win->y = msg->y;
			return true;
		case WINDOW_LIB_MESSAGE_CREATE:
			window_send_message_simple(win, WINDOW_LIB_MESSAGE_PREDRAW);
			break;		
		case WINDOW_LIB_MESSAGE_PREDRAW:
			on_window_predraw(win);
			window_send_message_simple(win, WINDOW_LIB_MESSAGE_DRAW);
			return true;
		case WINDOW_LIB_MESSAGE_DRAW:
			window_send_message_simple(win, WINDOW_LIB_MESSAGE_PRESENT);
			return true;
		case WINDOW_LIB_MESSAGE_PRESENT:
			syscall(SYSCall_windows_present, win->handle, WINDOW_EXT(win)->context);
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
			window_send_message_simple(win, WINDOW_LIB_MESSAGE_CREATE);
			break;
	}
}

context_t *window_context_create(int width, int height, int bpp) {
	int context_size = width * height * (bpp / 8);
	context_t *context = malloc(sizeof(context_t));

	context->width = width;
	context->height = height;
	context->bpp = bpp;
	context->buffer = malloc(context_size);
	memset(context->buffer, 0xFF, context_size);

	return context;
}

window_t *window_create(int x, int y, int width, int height, char* title) {
	window_t *window = calloc(sizeof(window_t), 1);

	// Init window structure
	window->x = x;
	window->y = y;
	window->height = height;
	window->width = width;
	window->window_default_procedure = &window_default_procedure;
	window->title = strdup(title);

	// Create Ext params
	window_ext_t *ext = malloc(sizeof(window_ext_t));
	ext->context = window_context_create(width, height, 32);
	window->ext = ext;

	// Create window (systemcall)
	window->handle = syscall(SYSCall_windows_create, x, y, width, height, title);

	// Set state to be created
	window_change_state(window, WINDOW_STATE_CREATED);

	return window;
}

void on_window_predraw(window_t *win) {
	int x1 = 0;
	int y1 = 0;
	int x2 = WINDOW_EXT(win)->context->width;
	int y2 = WINDOW_EXT(win)->context->height;

	// Main background
	gfx_rect(WINDOW_EXT(win)->context, x1, y1, x2, y2, WIN_BACKGROUND_COLOR);
	// Draw frame border
	gfx_rect(WINDOW_EXT(win)->context, x1, x2, y1, y2, WIN_FRAME_COLOR);

	// TODO syscall - syscall_windows_is_top(win->handle)
	bool is_top = false;
	uint32_t top_frame_color = is_top ? WIN_ACTIVE_FRAME : WIN_INACTIVE_FRAME;
	gfx_fillrect(WINDOW_EXT(win)->context, x1 + 1, y1 + 1, x2 - 1, y1 + WINDOW_BAR_HEIGHT, top_frame_color);

	// Frame label
	gfx_puts(WINDOW_EXT(win)->context, x1 + 1 + 3, y1 + 1 + 3, WIN_FRAME_TEXT_COLOR, top_frame_color, win->title);
}

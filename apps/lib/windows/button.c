#include <windows.h>
#include <windows/messaging.h>
#include <windows/button.h>
#include <klika_os.h>
#include <syscalls.h>
#include <stdbool.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <gfx.h>


button_t *button_create(window_t *parent, int x, int y, int width, int height, char* title, int id) {
	button_t *window = calloc(sizeof(button_t), 1);

	// Init window structure
	window->type = WINDOW_TYPE_BUTTON;
	window->id = id;
	window->x = x;
	window->y = y;
	window->height = height;
	window->width = width;
	window->default_procedure = &button_default_procedure;
	window->title = strdup(title);
	window->parent = parent;
	window->state = BUTTON_STATE_NORMAL;

	window_add_child(parent, window);

	// Create Ext params
	// window_ext_t *ext = calloc(sizeof(window_ext_t), 1);
	// ext->context = window_context_create(width, height, 32);
	// window->ext = ext;

	// Create window (systemcall)
	// window->handle = syscall(SYSCall_windows_create, x, y, width, height, title);

	// Set state to be created
	// window_change_state(window, WINDOW_STATE_CREATED);

	return window;	
}

bool button_default_procedure(button_t *win, struct message_struct *msg) {
	switch(msg->message) {
		case MESSAGE_MOUSE_PRESS:
			if (WINDOW_EXT(win->parent)->focused == win) {
				win->state = BUTTON_STATE_PRESS;
				window_invalidate(win->parent);
				return true;
			}
			return false;
		case MESSAGE_MOUSE_RELEASE:
			if (WINDOW_EXT(win->parent)->focused == win) {
				win->state = BUTTON_STATE_NORMAL;
				window_invalidate(win->parent);
				window_send_message_simple(win->parent, win->id);
				return true;
			}
			return false;
		case WINDOW_LIB_MESSAGE_CREATE:
			window_dispatch_message_simple(win, WINDOW_LIB_MESSAGE_PREDRAW);
			return false;
		case WINDOW_LIB_MESSAGE_PREDRAW:
			on_button_predraw(win);
			window_dispatch_message_simple(win, WINDOW_LIB_MESSAGE_DRAW);
			return false;
		case WINDOW_LIB_MESSAGE_DRAW:
			window_dispatch_message_simple(win, WINDOW_LIB_MESSAGE_PRESENT);
			return false;
	}
	return false;
}


void on_button_predraw(button_t *win) {
	int x1 = win->x;
	int y1 = win->y;
	int x2 = x1 + win->width;
	int y2 = y1 + win->height;

	// Main background
	uint32_t bg_color = win->state == BUTTON_STATE_PRESS ? BTN_BACKGROUND_PRESS_COLOR : BTN_BACKGROUND_COLOR;
	uint32_t fr_color = win->state == BUTTON_STATE_PRESS ? BTN_FRAME_PRESS_COLOR : BTN_FRAME_COLOR;
	gfx_fillrect(WINDOW_EXT(win->parent)->context, x1, y1, x2, y2, bg_color);
	gfx_rect(WINDOW_EXT(win->parent)->context, x1, y1, x2, y2, fr_color);

	// Frame label
	int text_x = x1 + (win->width - TEXT_FONT_WIDTH(win->title) ) / 2;
	int text_y = y1 + (win->height - TEXT_FONT_HEIGHT(win->title) ) / 2;
	gfx_puts(WINDOW_EXT(win->parent)->context, text_x, text_y, BTN_TEXT_COLOR, bg_color, win->title);
}

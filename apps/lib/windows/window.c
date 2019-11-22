#include <windows.h>
#include <klika_os.h>
#include <syscalls.h>
#include <stdbool.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <gfx.h>

static window_t *window_find_focused_component(window_t *win, message_t *msg);

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
	memset(context->buffer, 0xF4, context_size); // f4f4f4 - WINDOW_BACKGROUND_COLOR

	return context;
}

window_t *window_create(int x, int y, int width, int height, char* title, int id) {
	window_t *window = calloc(sizeof(window_t), 1);

	// Init window structure
	window->type = WINDOW_TYPE_WINDOW;
	window->id = id;
	window->x = x;
	window->y = y;
	window->height = height;
	window->width = width;
	window->default_procedure = &window_default_procedure;
	window->title = strdup(title);

	// Create Ext params
	window_ext_t *ext = calloc(sizeof(window_ext_t), 1);
	ext->context = window_context_create(width, height, 32);
	window->ext = ext;

	// Create window (systemcall)
	window->handle = syscall(SYSCall_windows_create, x, y, width, height, title);

	// Set state to be created
	window_change_state(window, WINDOW_STATE_CREATED);

	// Create Close and Min buttons
	WINDOW_EXT(window)->button_close = button_create(window, 1, 1, 12, 12, "X", WINDOW_LIB_BUTTON_CLOSE);
	WINDOW_EXT(window)->button_min = button_create(window, 16, 1, 12, 12, "_", WINDOW_LIB_BUTTON_MIN);

	return window;
}

void on_window_predraw(window_t *win) {
	int x1 = 0;
	int y1 = 0;
	int x2 = WINDOW_EXT(win)->context->width;
	int y2 = WINDOW_EXT(win)->context->height;

	// Main background
	gfx_fillrect(WINDOW_EXT(win)->context, x1, y1, x2, y2, WIN_BACKGROUND_COLOR);
	// Draw frame border
	gfx_rect(WINDOW_EXT(win)->context, x1, y1, x2, y2, WIN_FRAME_COLOR);

	// TODO syscall - syscall_windows_is_top(win->handle)
	bool is_top = false;
	uint32_t top_frame_color = is_top ? WIN_ACTIVE_FRAME : WIN_INACTIVE_FRAME;
	gfx_fillrect(WINDOW_EXT(win)->context, x1 + 1, y1 + 1, x2 - 1, y1 + WINDOW_BAR_HEIGHT, top_frame_color);

	// Frame label
	int text_x = (win->width - TEXT_FONT_WIDTH(win->title)) / 2;
	gfx_puts(WINDOW_EXT(win)->context, x1 + text_x, y1 + 1 + 3, WIN_FRAME_TEXT_COLOR, top_frame_color, win->title);
}

void window_add_child(window_t *parent, window_t *child) {
	window_t *w = parent;
	while (w->next != NULL) { w = w->next; }
	w->next = child;
}

bool window_point_inside(window_t* win, int x, int y) {
	return x >= win->x && x <= win->x + win->width &&
				 y >= win->y && y <= win->y + win->height;
}

bool window_dispatch_message(window_t *win, struct message_struct *msg) {
	bool consumed = false;
	if (win->window_procedure != NULL) {
		consumed = win->window_procedure(win, msg);
	}
	if (!consumed) {
		consumed = win->default_procedure(win, msg);
	}
	return consumed;
}

bool window_dispatch_message_simple(window_t *win, int message) {
	message_t msg;
	msg.message = message;
	return window_dispatch_message(win, &msg);
}

void window_dispatch(window_t *win, struct message_struct *msg) {
	for (window_t *w = win; w != NULL; w = w->next) {
		// If message is consumed, stop the chain
		if (window_dispatch_message(w, msg)) {
			break;
		}
	}
}

bool window_default_procedure(window_t *win, message_t *msg) {
	switch(msg->message) {
		case MESSAGE_MOUSE_PRESS:
		case MESSAGE_MOUSE_RELEASE:
			WINDOW_EXT(win)->focused = window_find_focused_component(win, msg);
			return false;
		case MESSAGE_WINDOW_DRAG:
			win->x = msg->x;
			win->y = msg->y;
			return true;
		case WINDOW_LIB_MESSAGE_CREATE:
			window_send_message_simple(win, WINDOW_LIB_MESSAGE_PREDRAW);
			return false;
		case WINDOW_LIB_MESSAGE_PREDRAW:
			on_window_predraw(win);
			window_send_message_simple(win, WINDOW_LIB_MESSAGE_DRAW);
			return false;
		case WINDOW_LIB_MESSAGE_DRAW:
			window_send_message_simple(win, WINDOW_LIB_MESSAGE_PRESENT);
			return false;
		case WINDOW_LIB_MESSAGE_PRESENT:
			syscall(SYSCall_windows_present, win->handle, WINDOW_EXT(win)->context);
			return true;
		case WINDOW_LIB_BUTTON_CLOSE:
			DEBUG("CLOSE BUTTON PRESSED");
			return true;
		case WINDOW_LIB_BUTTON_MIN:
			DEBUG("MIN BUTTON PRESSED");
			return true;
	}
	return false;
}

vector_t mouse_to_local(window_t *win, message_t *msg) {
	return (vector_t){msg->x - win->x, msg->y - win->y};
}

static window_t *window_find_focused_component(window_t *win, message_t *msg) {
	vector_t local = mouse_to_local(win, msg);
	for(window_t *w = win->next; w != NULL; w = w->next) {
		if (window_point_inside(w, local.x, local.y)) {
			return w;
		}
	}
	return NULL;
}

void window_invalidate(window_t *win) {
	window_send_message_simple(win, WINDOW_LIB_MESSAGE_PREDRAW);
}
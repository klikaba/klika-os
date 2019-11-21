#include <windows.h>
#include <windows/messaging.h>
#include <windows/label.h>
#include <klika_os.h>
#include <syscalls.h>
#include <stdbool.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <gfx.h>


window_t *label_create(window_t *parent, int x, int y, int width, int height, char* title, int id) {
	window_t *window = calloc(sizeof(window_t), 1);

	// Init window structure
	window->type = WINDOW_TYPE_LABEL;
	window->id = id;
	window->x = x;
	window->y = y;
	window->height = height;
	window->width = width;
	window->default_procedure = &label_default_procedure;
	window->title = strdup(title);
	window->parent = parent;

	window_add_child(parent, window);
	return window;	
}

void label_set_text(window_t *label, const char *text) {
	free(label->title);
	label->title = strdup(text);
	window_invalidate(label->parent);
}

bool label_default_procedure(window_t *label, struct message_struct *msg) {
	switch(msg->message) {
		case WINDOW_LIB_MESSAGE_CREATE:
			window_dispatch_message_simple(label, WINDOW_LIB_MESSAGE_PREDRAW);
			break;
		case WINDOW_LIB_MESSAGE_PREDRAW:
			on_label_predraw(label);
			window_dispatch_message_simple(label, WINDOW_LIB_MESSAGE_DRAW);
			break;
		case WINDOW_LIB_MESSAGE_DRAW:
			window_dispatch_message_simple(label, WINDOW_LIB_MESSAGE_PRESENT);
			break;
	}
	return false;
}

void on_label_predraw(window_t *label) {
	int x1 = label->x;
	int y1 = label->y;
	int x2 = x1 + label->width;
	int y2 = y1 + label->height;

	gfx_fillrect(WINDOW_EXT(label->parent)->context, x1, y1, x2, y2, LABEL_BACKGROUND_COLOR);
	gfx_puts(WINDOW_EXT(label->parent)->context, label->x, label->y, LABEL_TEXT_COLOR, LABEL_BACKGROUND_COLOR, label->title);
}

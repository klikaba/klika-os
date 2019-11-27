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
#include <bmp.h>

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

	// Create Ext params
	window->ext = calloc(sizeof(button_ext_t), 1);

	window_add_child(parent, window);

	return window;	
}

bmp_image_t* button_get_image(button_t *button) {
	if (!BUTTON_EXT(button)->bmp_present) {
		return NULL;
	}

	int state = button->state;
	bmp_image_t * bmp_press = BUTTON_EXT(button)->bmp_press;
	bmp_image_t * bmp_normal = BUTTON_EXT(button)->bmp_normal;

	// return specific state, if not return other
	if (state == BUTTON_STATE_PRESS) {
		return bmp_press != NULL ? bmp_press : bmp_normal;
	}
	else {
		return bmp_normal != NULL ? bmp_normal : bmp_press;
	}
}

void button_set_image(button_t *button, int state, bmp_image_t *bmp) {
	switch(state) {
		case BUTTON_STATE_NORMAL:
			BUTTON_EXT(button)->bmp_normal = bmp;
			break;
		case BUTTON_STATE_PRESS:
			BUTTON_EXT(button)->bmp_press = bmp;
			break;
	}
	BUTTON_EXT(button)->bmp_present = (BUTTON_EXT(button)->bmp_press != NULL || BUTTON_EXT(button)->bmp_normal != NULL);
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

static void draw_button_frame(button_t *win) {
	int x1 = win->x;
	int y1 = win->y;
	int x2 = x1 + win->width;
	int y2 = y1 + win->height;
	int shift = 0;

	if (win->state == BUTTON_STATE_PRESS) {
		gfx_rect_width(WINDOW_EXT(win->parent)->context, x1+1, y1+1, x2, y2, BTN_FRAME_COLOR, WINDOW_FRAME_WIDTH);
		gfx_fillrect(WINDOW_EXT(win->parent)->context, x1+3, y1+3, x2-2, y2-2, BTN_BACKGROUND_COLOR);		
		shift = 1;
	}
	else {
		gfx_draw_shadowed_box(WINDOW_EXT(win->parent)->context, x1, y1, x2, y2, BTN_FRAME_COLOR, BTN_BACKGROUND_COLOR);
	}

	int text_x = x1 + (win->width - TEXT_FONT_WIDTH(win->title) ) / 2 + shift;
	int text_y = y1 + (win->height - TEXT_FONT_HEIGHT(win->title) ) / 2 + shift;
	gfx_puts(WINDOW_EXT(win->parent)->context, text_x, text_y, BTN_TEXT_COLOR, BTN_BACKGROUND_COLOR, win->title);
}

static void draw_button_with_image(button_t *win) {
	int x1 = win->x;
	int y1 = win->y;
	int x2 = x1 + win->width-1;
	int y2 = y1 + win->height-1;

	// Main background
	gfx_fillrect(WINDOW_EXT(win->parent)->context, x1, y1, x2, y2, WIN_BACKGROUND_COLOR);

	int shift = win->state == BUTTON_STATE_NORMAL ? 0 : 1;
	bmp_image_t *bmp = button_get_image(win);
	gfx_blit(WINDOW_EXT(win->parent)->context, x1 + shift, y1 + shift, bmp->header->width_px, bmp->header->height_px, bmp->data);

	// Frame label @ bottom
	int text_x = x1 + (win->width - TEXT_FONT_WIDTH(win->title) ) / 2;
	int text_y = y2 - TEXT_FONT_HEIGHT(win->title);
	gfx_puts(WINDOW_EXT(win->parent)->context, text_x + shift, text_y + shift, 0, 0xFFFFFF, win->title);
}

void on_button_predraw(button_t *win) {
	if (BUTTON_EXT(win)->bmp_present) {
		draw_button_with_image(win);
	}
	else {
		draw_button_frame(win);
	}
}

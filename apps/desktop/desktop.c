#include <klika_os.h>
#include <windows.h>
#include <gfx.h>
#include <bmp.h>

message_t msg;
window_t  *window;

#define MSG_USER_WIN (WINDOW_USER_MESSAGE + 1)
#define MSG_USER_BTN_SIMPLE_WIN (WINDOW_USER_MESSAGE + 2)

#include "app_icon.bmp.h"

#include "../simple_win/build/simple_win_byte.c"

bmp_image_t* app_icon_bmp;

void start_simple_win() {
	syscall(SYSCall_process_create, simple_win);
}

bmp_image_t* bmp_read_from_memory(void* bmp_file) {
	bmp_image_t* bmp = (bmp_image_t*)bmp_file;
	bmp->data = (uint32_t*)(((uint8_t*)bmp_file) + bmp->header.offset);
	return bmp;
}


void draw_button_with_icon(button_t *win) {
	int x1 = win->x;
	int y1 = win->y;
	int x2 = x1 + win->width;
	int y2 = y1 + win->height;

	// Main background
	gfx_fillrect(WINDOW_EXT(win->parent)->context, x1, y1, x2, y2, WIN_BACKGROUND_COLOR);

	int shift = win->state == BUTTON_STATE_NORMAL ? 0 : 1;
	gfx_blit(WINDOW_EXT(win->parent)->context, x1 + shift, y1 + shift, 64, 64, app_icon_bmp->data);

	// Frame label
	int text_x = x1 + (win->width - TEXT_FONT_WIDTH(win->title) ) / 2;
	int text_y = y2 - TEXT_FONT_HEIGHT(win->title);
	gfx_puts(WINDOW_EXT(win->parent)->context, text_x + shift, text_y + shift, 0, 0xFFFFFF, win->title);
}

bool button_procedure(button_t *button, struct message_struct *msg) {
	if (msg->message == WINDOW_LIB_MESSAGE_DRAW) {
		draw_button_with_icon(button);
	}
	return false;
}


int main() {
	app_icon_bmp = bmp_read_from_memory(app_icon_data);
	window = window_create(0, 768 - 100, 1024, 100, "Applications", MSG_USER_WIN);
	button_t *btn = button_create(window, 10, 20, 64, 64 + 9, "Simple", MSG_USER_BTN_SIMPLE_WIN);
	btn->window_procedure = &button_procedure;

	while(window_get_message(window, &msg)) { 
		switch(msg.message) {
			case MSG_USER_BTN_SIMPLE_WIN:
				start_simple_win();				
				break;
		}
		window_dispatch(window, &msg);
	}
	return 0;
}
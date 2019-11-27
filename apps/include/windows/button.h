#pragma once

#include <string.h>
#include <bmp.h>

#define WINDOW_TYPE_BUTTON 1

#define BUTTON_STATE_NORMAL  0
#define BUTTON_STATE_PRESS   1

#define BTN_BACKGROUND_COLOR 				0xCCCCCC
#define BTN_FRAME_COLOR 						0x333333

#define BTN_TEXT_COLOR 							0x000033

#define TEXT_FONT_WIDTH(str) (strlen(str)*8)
#define TEXT_FONT_HEIGHT(str) (8)

#define button_t window_t

#define BUTTON_EXT(button) ((button_ext_t*)button->ext)

typedef struct {
	bool bmp_present;
	bmp_image_t *bmp_normal;
	bmp_image_t *bmp_press;
} button_ext_t;

button_t *button_create(window_t *parent, int x, int y, int width, int height, char* title, int id);
bool button_default_procedure(button_t *win, struct message_struct *msg);
void on_button_predraw(button_t *win);
void button_set_image(button_t *button, int state, bmp_image_t *bmp);
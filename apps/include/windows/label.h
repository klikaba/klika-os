#pragma once

#define WINDOW_TYPE_LABEL 2

#define LABEL_TEXT_COLOR 			 0x000000
#define LABEL_BACKGROUND_COLOR 0xFFFFFF

#define label_t window_t

label_t *label_create(window_t *parent, int x, int y, int width, int height, char* title, int id);
bool label_default_procedure(label_t *label, struct message_struct *msg);
void label_set_text(label_t *label, const char *text);
void on_label_predraw(label_t *label);
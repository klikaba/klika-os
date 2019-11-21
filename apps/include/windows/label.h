#pragma once

#define WINDOW_TYPE_LABEL 2

#define LABEL_TEXT_COLOR 0x000000
#define LABEL_BACKGROUND_COLOR 0xFFFFFF

#define label_t window_t

window_t *label_create(window_t *parent, int x, int y, int width, int height, char* title, int id);
bool label_default_procedure(window_t *label, struct message_struct *msg);
void label_set_text(window_t *label, const char *text);
void on_label_predraw(window_t *label);
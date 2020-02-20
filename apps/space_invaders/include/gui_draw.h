#ifndef __GUI_DRAW_H
#define __GUI_DRAW_H

#include "windows.h"

void draw_background(context_t *context);
void draw_header(context_t *context, char *score_label, char *hiscore_label);
void draw_footer(context_t *context, bmp_image_t *sprite, int lives, int width, int height);
void draw_text(context_t *context, int x, int y, const char *text);
void draw_game_over(context_t *context);

#endif

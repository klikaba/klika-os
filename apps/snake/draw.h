#ifndef __draw_H
#define __draw_H

#include <windows.h>
#include <gfx.h>

#include "util.h"

void draw_object(context_t *context, int x, int y, uint32_t color) {
	gfx_fillrect(context, (x + 0.1) * 20, (y + 2.2) * 20, ((x + 0.1) * 20) + 20, ((y + 2.2) * 20) + 20, color);
}

void draw_background(context_t *context, window_t  *window) {
	gfx_fillrect(context, 2, 43, window->width - 5, window->height - 55, BACKGROUND_COLOR);
}

void draw_footer(context_t *context, window_t  *window) {
	gfx_fillrect(context, 2, window->height - 55, window->width - 5, window->height - 5, FOOTER_COLOR);
}

void draw_scores(context_t *context, const char *score, const char *hiscore) {
	gfx_puts_trans(context, 300, 458, LABEL_COLOR, score);
	gfx_puts_trans(context, 300, 478, LABEL_COLOR, hiscore);
}

void draw_menu(context_t *context, bool sub_menu) {
	gfx_fillrect(context, 15, 455, 120, 485, BUTTON_COLOR);
	gfx_puts_trans(context, 35, 468, LABEL_COLOR, "NEW GAME");

	if (sub_menu) {
		gfx_fillrect(context, 135, 200, 270, 230, BUTTON_COLOR);
		gfx_puts_trans(context, 135 + 40, 213, LABEL_COLOR, "CLASSIC");

		gfx_fillrect(context, 135, 240, 270, 270, BUTTON_COLOR);
		gfx_puts_trans(context, 135 + 37, 253, LABEL_COLOR, "NO WALLS");

		gfx_fillrect(context, 135, 280, 270, 310, BUTTON_COLOR);
		gfx_puts_trans(context, 135 + 32, 293, LABEL_COLOR, "OBSTACLES");
	}
}

#endif
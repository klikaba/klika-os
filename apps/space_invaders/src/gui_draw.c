#include "gui_draw.h"
#include "constants.h"

#include <stdlib.h>
#include <gfx.h>
#include <bmp.h>

void draw_background(context_t *context) {
	gfx_fillrect(context, 0, WINDOW_TITLEBAR_HEIGHT, WINDOW_WIDTH, WINDOW_HEIGHT, COLOR_BLACK);	
}

void draw_text(context_t *context, int x, int y, const char *text) {
	gfx_puts(context, x, y, COLOR_WHITE, COLOR_BLACK, text);
}

void draw_header(context_t *context, char *score_label, char *hiscore_label) {
	draw_text(context, 25, 50, "S C O R E < 1 >");
    draw_text(context, 165, 50, "H I - S C O R E");
	draw_text(context, 305, 50, "S C O R E < 2 >");
	
    draw_text(context, 65, 70, score_label);
    draw_text(context, 205, 70, hiscore_label);
	draw_text(context, 345, 70, "0000");
}

void draw_game_over(context_t *context) {
	draw_text(context, 150, 95, "G A M E  O V E R");
	draw_text(context, 110, 112, "[ PRESS ENTER TO CONTINUE ]");
}

void draw_footer(context_t *context, bmp_image_t *sprite, int lives, int width, int height) {
	char player_lives_left[2];
	sprintf(player_lives_left, "%01i", lives);

	gfx_hline(context, 0, WINDOW_WIDTH, BOTTOM_BOUND, COLOR_GREEN);
	draw_text(context, 20, WINDOW_HEIGHT - 25, player_lives_left);
	
	for (int i = 0; i < lives - 1; i++) {
		int x = 35 + 7 * (i + 1) + width * i;
		int y = WINDOW_HEIGHT - height - 15;

		bmp_blit_clipped(context, sprite, x, y, 0, 0, 28, 17);
	}
}

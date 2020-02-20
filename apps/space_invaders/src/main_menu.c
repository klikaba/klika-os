#include "main_menu.h"
#include "constants.h"
#include "gui_draw.h"

static window_t *window;
static context_t *context;
static message_t msg;

static bmp_image_t spritesheet;

int press_enter_ticks_limit;
int press_enter_ticks_count;
int press_enter_visible;

static MenuButton buttons[] = {
    MenuButton(190, 180, 150, 100, "P L A Y", STATE_NEW_GAME)
};

static int id = STATE_MAIN_MENU;
static int main_menu_id(void) {
   return id;
}

static void main_menu_init(void) {
    bmp_from_file("/apps/invaders/sprites.bmp", &spritesheet);

    press_enter_ticks_limit = 90;
    press_enter_ticks_count = 0;
    press_enter_visible = 1;
}

static int main_menu_update(void) {
    int next_state = main_menu_id();

    window_peek_message(window, &msg);

	if (msg.message != 0) {
		if (msg.message == MESSAGE_KEY_PRESS && msg.key == KEY_RETURN) {
            next_state = buttons[0].state;
		}

		window_dispatch(window, &msg);
	}
    
    if (press_enter_ticks_count++ >= press_enter_ticks_limit) {
        press_enter_visible = !press_enter_visible;
        press_enter_ticks_count = 0;
    } else {
        press_enter_ticks_count++;
    }

    return next_state;
}

static void main_menu_draw(void) {
	draw_background(context);
    
    // TODO: Add player hi-score loading from file
    draw_header(context, "0000", "0000");

    draw_text(context, buttons[0].x, buttons[0].y, buttons[0].text);

    draw_text(context, 120, 210, "S P A C E   I N V A D E R S");
    
    if (press_enter_visible) {
        draw_text(context, 130, 250, "[ PRESS ENTER TO START ]");
    }

    draw_text(context, 128, 290, "* SCORE  ADVANCE  TABLE *");

    bmp_blit_clipped(context, &spritesheet, 156, 320, 152, 43, 31, 14);
    draw_text(context, 194, 322, "= ?   MYSTERY");

    bmp_blit_clipped(context, &spritesheet, 164, 350, 106, 0, 16, 16);
    draw_text(context, 194, 355, "= 30   POINTS");

    bmp_blit_clipped(context, &spritesheet, 163, 380, 154, 0, 22, 16);
    draw_text(context, 194, 385, "= 20   POINTS");
    
    bmp_blit_clipped(context, &spritesheet, 160, 410, 213, 0, 24, 16);
    draw_text(context, 194, 415, "= 10   POINTS");

    window_present(window);
}

static void main_menu_deinit(void) {
}

void create_main_menu(GameState *state, window_t *win, context_t *ctx) {
    state->id = main_menu_id;
    state->init = main_menu_init;
    state->update = main_menu_update;
    state->draw = main_menu_draw;
    state->deinit = main_menu_deinit;

    window = win;
    context = ctx;
}

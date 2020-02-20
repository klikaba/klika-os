#ifndef __MAIN_MENU_H__
#define __MAIN_MENU_H__

#include "game_state.h"
#include <windows.h>
#include <gfx.h>
#include <bmp.h>

typedef struct {
    int x;
    int y;
    int width;
    int height;
    char *text;
    int state;
} MenuButton;

#define MenuButton(x, y, width, height, text, state) {x, y, width, height, text, state}

void create_main_menu(GameState * state, window_t *win, context_t *ctx);

#endif

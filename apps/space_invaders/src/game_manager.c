#include <windows.h>

#include "game_manager.h"
#include "constants.h"
#include "main_menu.h"
#include "game.h"

#define MSG_USER_WIN    (WINDOW_USER_MESSAGE + 1)

window_t  *window;
context_t *context;

static GameState game_states[GAME_STATES_COUNT];
static int current_state_index = 0;

void game_manager_init(void) {
    window = window_create(WINDOW_POSITION_X, WINDOW_POSITION_Y, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, MSG_USER_WIN, WINDOW_ATTR_NONE, WINDOW_FRAME_DEFAULT);
	context = WINDOW_EXT(window)->context;

    create_main_menu(&game_states[0], window, context);
    create_new_game(&game_states[1], window, context);
}

void game_manager_draw(void) {
    game_states[current_state_index].draw();
}

void game_manager_run(void) {
    game_states[current_state_index].init();

    while (1) {
        int nextState = game_states[current_state_index].update();

        if (nextState == STATE_QUIT_GAME) {
            current_state_index = STATE_QUIT_GAME;
            break;
        } else if (nextState == STATE_GAME_OVER) {
            game_states[current_state_index].deinit();

            current_state_index = 0;
            game_states[current_state_index].init();
        }
        else if (nextState != game_states[current_state_index].id()) {
            for (int i = 0; i < GAME_STATES_COUNT; i++) {
                if (game_states[i].id() == nextState) {
                    game_states[current_state_index].deinit();
                    current_state_index = i;
                    game_states[current_state_index].init();
                    break;
                }
            }
        }

        game_manager_draw();
    }
}

void game_manager_shutdown(void) { 
    window_close(window, 0);
}

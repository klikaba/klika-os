#include <stdlib.h>
#include "game.h"
#include "gui_draw.h"
#include "constants.h"

#include "player.h"
#include "alien.h"
#include "shot.h"
#include "shield.h"
#include "flying_saucer.h"

static window_t  *window;
static context_t *context;
static message_t msg;

static bmp_image_t spritesheet;

static char score_label[8];
static char hiscore_label[8];

static GameObject left_bound;
static GameObject top_bound;
static GameObject right_bound;
static GameObject bottom_bound;

static Player player;
static Shot player_shot;

static Alien aliens[ALIENS_COUNT];
static Shot alien_shots[3];

static FlyingSaucer flying_saucer;
static Shield shields[SHIELDS_COUNT];

// Status flags
int game_paused;
int game_over;
int quit_game;
int alien_exploding;
int aliens_reached_bottom;
int aliens_switch_direction;
int aliens_update_move_speed;
int aliens_spawn_drawn;

int aliens_spawns_count;
int aliens_alive_count;
int aliens_spawn_draw_index;

int player_explode_trans_ticks;
int alien_explode_trans_ticks;
int alien_rolling_shot_ticks;

// TODO: Alien - shield: on direct collision remove shield

static int _id = STATE_NEW_GAME;
static int game_id(void) {
   return _id;
}

static void status_flags_init(void) {
	game_paused = 1;
	game_over = 0;
	quit_game = 0;
	alien_exploding = -1;
	aliens_reached_bottom = 0;
	aliens_switch_direction = 0;
	aliens_update_move_speed = 0;
	aliens_spawn_drawn = 0;
	
	aliens_spawns_count = 1;
	aliens_alive_count = 0;
	aliens_spawn_draw_index = 0;

	player_explode_trans_ticks = 0;
	alien_explode_trans_ticks = 0;
	alien_rolling_shot_ticks = 0;
}

static void sprites_init(void) {
	bmp_from_file("/apps/invaders/sprites.bmp", &spritesheet);
}

static void game_init(void) {
	status_flags_init();
	sprites_init();
	srand(0);

	left_bound = (GameObject) { 0, 0, LEFT_BOUND, WINDOW_HEIGHT, 0, 0, 0, 1, 0, 0, 0, 0 };
	top_bound = (GameObject) { 0, 0, WINDOW_WIDTH, WINDOW_TITLEBAR_HEIGHT + 45, 0, 0, 0, 1, 0, 0, 0, 0 };
	right_bound = (GameObject) { RIGHT_BOUND, 0, WINDOW_WIDTH - RIGHT_BOUND, WINDOW_HEIGHT, 0, 0, 0, 1, 0, 0, 0, 0 };
	bottom_bound = (GameObject) { 0, BOTTOM_BOUND, WINDOW_WIDTH, WINDOW_HEIGHT - BOTTOM_BOUND, 0, 0, 0, 1, 0, 0, 0, 0 };
	
	player_init(&player, &spritesheet);
	aliens_init(aliens, &spritesheet, get_alien_spawn_y(aliens_spawns_count));
	flying_saucer_init(&flying_saucer, &spritesheet);
	
	shot_init(&player_shot, &spritesheet, PLAYER_SHOT);
	shot_init(&alien_shots[0], &spritesheet, PLUNGER_SHOT);
	shot_init(&alien_shots[1], &spritesheet, ROLLING_SHOT);
	shot_init(&alien_shots[2], &spritesheet, SQUIGGLY_SHOT);

	shields_init(shields);

	sprintf(score_label, "%04i", player.score);
	sprintf(hiscore_label, "%04i", player.hiscore);
}

static void handle_user_input(message_t *msg) {
	if (msg->message == MESSAGE_KEY_PRESS) {
		if (game_paused && game_over && msg->key == KEY_RETURN) {
			quit_game = 1;
		} else if (!game_paused) {
			switch (msg->key) {
				case KEY_LEFT:
					player.game_object.base.move_direction_x = MOVE_LEFT_DIRECTION;
					break;
				case KEY_RIGHT:
					player.game_object.base.move_direction_x = MOVE_RIGHT_DIRECTION;
					break;
				case KEY_SPACE: {
					if (!player_shot.game_object.base.is_enabled) {
						int x = player.game_object.base.x + (player.game_object.base.width / 2) - 1;
						int y = player.game_object.base.y;

						shot_fire(&player_shot, x, y, MOVE_UP_DIRECTION);
					}
					break;
				}
			}
		}
	} 
	else if (msg->message == MESSAGE_KEY_RELEASE) {
		switch (msg->key) {
			case KEY_LEFT:
				if (player.game_object.base.move_direction_x == MOVE_LEFT_DIRECTION)
					player.game_object.base.move_direction_x = NO_MOVEMENT;
				break;
			case KEY_RIGHT:
				if (player.game_object.base.move_direction_x == MOVE_RIGHT_DIRECTION)
					player.game_object.base.move_direction_x = NO_MOVEMENT;
				break;
		}
	}
}

static void user_input_check(void) {
	window_peek_message(window, &msg);

	if (msg.message != 0) {
		handle_user_input(&msg);
		window_dispatch(window, &msg);
	}
}

static int game_update(void) {
	if (!aliens_spawn_drawn && aliens_spawn_draw_index == ALIENS_COUNT) {
		game_paused = 0;
		aliens_spawn_drawn = 1;
	}

	user_input_check();

	if (game_over && quit_game) {
		return STATE_GAME_OVER;
	}

	if (player_explode_trans_ticks > 0) {
		--player_explode_trans_ticks;
	}
	else {
		if (player.game_object.base.is_enabled == 0) {
			if (player.lives == 0) {
				game_over = 1;
			} else {
				player_reinit(&player);
				game_paused = 0;
			}
		} 
	}

	if (alien_explode_trans_ticks > 0) {
		--alien_explode_trans_ticks;
	} else {
		if (alien_exploding >= 0) {
			alien_exploding = -1;
		}
	}

	update(&player.game_object.base);
	animate(&player.game_object);

	if (player_shot.game_object.base.is_visible) {
		update(&player_shot.game_object.base);
		animate(&player_shot.game_object);

		if (collides(&player_shot.game_object.base, &top_bound)) {
			shot_explode(&player_shot);
		} 
		else if (flying_saucer.game_object.base.is_visible && collides(&player_shot.game_object.base, &flying_saucer.game_object.base)) {
			flying_saucer_explode(&flying_saucer);
			player_shot.game_object.base.is_enabled = 0;
			player_shot.game_object.base.is_visible = 0;

			player.score += flying_saucer_get_points();
		}
		else {
			// Check collision with shields
			for (int i = 0; i < SHIELDS_COUNT; i++) {
				if (pixel_collides(&shields[i], &player_shot)) {
					player_shot.game_object.base.move_direction_y = 0;
					player_shot.game_object.base.is_enabled = 0;
					player_shot.game_object.base.is_visible = 0;
					break;
				}
			}

			// Check collision with aliens
			for (int i = 0; i < ALIENS_COUNT; i++) {
				if (collides(&aliens[i].game_object.base, &player_shot.game_object.base)) {
					alien_explode(&aliens[i]);
					
					aliens_update_move_speed = 1;
					alien_explode_trans_ticks = 5;
					alien_exploding = i;

					player_shot.game_object.base.is_enabled = 0;
					player_shot.game_object.base.is_visible = 0;
					
					player.score += aliens[i].points;
					break;
				}
			}
		}
	}

	if (alien_exploding >= 0) {
		update(&aliens[alien_exploding].game_object.base);
		animate(&aliens[alien_exploding].game_object);
		return game_id();
	}

	if (!game_paused) {
		if (player.game_object.base.move_direction_x == MOVE_LEFT_DIRECTION) {
			if (collides(&player.game_object.base, &left_bound)) {
				player.game_object.base.move_direction_x = NO_MOVEMENT;
				player.game_object.base.x = LEFT_BOUND;
			}
		}
		else if (player.game_object.base.move_direction_x == MOVE_RIGHT_DIRECTION) {
			if (collides(&player.game_object.base, &right_bound)) {
				player.game_object.base.move_direction_x = NO_MOVEMENT;
				player.game_object.base.x = RIGHT_BOUND - player.game_object.base.width;
			}
		}

		aliens_alive_count = 0;
		aliens_reached_bottom = 0;

		// Count alive aliens, and check if any reached bottom
		for (int i = 0; i < ALIENS_COUNT; i++) {
			if (aliens[i].game_object.base.is_visible) {
				++aliens_alive_count;

				if (aliens[i].game_object.base.y + aliens[i].game_object.base.height > 480) {
					aliens_reached_bottom = 1;
				}
			}
		}

		if (aliens_alive_count == 0) {
			aliens_init(aliens, &spritesheet, get_alien_spawn_y(++aliens_spawns_count));
			player_reinit(&player);
			aliens_spawn_draw_index = 0;
			aliens_spawn_drawn = 0;
			game_paused = 1;
			return game_id();
		}

		if (aliens_alive_count > 0) {
			if (aliens_reached_bottom) {
				player_lose_life(&player);
				player.lives = 0;
				player_explode_trans_ticks = 6 * 10;
				game_paused = 1;

				return game_id();
			}

			int aliens_moved = 0;

			for (int i = 0; i < ALIENS_COUNT; i++) {
				if (update(&aliens[i].game_object.base)) {
					aliens_moved = 1;
				}

				animate(&aliens[i].game_object);

				int move_dir = aliens[i].game_object.base.move_direction_x;
				if (collides(&aliens[i].game_object.base, move_dir > 0 ? &right_bound : &left_bound)) {
					aliens_switch_direction = 1;
				}
			}

			if (aliens_switch_direction) {
				for (int i = 0; i < ALIENS_COUNT; i++) {
					aliens[i].game_object.base.move_direction_x *= -1;
					aliens[i].game_object.base.y += 21;
				}
				
				aliens_switch_direction = 0;
			}

			if (aliens_update_move_speed && aliens_moved && alien_update_move_speed(aliens, aliens_alive_count)) {
				aliens_update_move_speed = 0;
			}

			if (alien_rolling_shot_ticks > 0) {
				--alien_rolling_shot_ticks;
			}

			// Try fire rolling shot
			if (alien_rolling_shot_ticks == 0) {
				if (alien_fire_shot(aliens, alien_shots, 1, player.game_object.base.x, player.score)) {
					alien_rolling_shot_ticks = alien_get_reload_ticks_count(player.score) * 2;
				}
			}

			// Try fire plunger shot
			alien_fire_shot(aliens, alien_shots, 0, player.game_object.base.x, player.score);

			// Try to fire squiggly shot or spawn flying saucer
			if (!alien_fire_shot(aliens, alien_shots, 2, player.game_object.base.x, player.score)) {
				if (flying_saucer_ready(&flying_saucer) && aliens_alive_count >= 8) {
					flying_saucer_spawn(&flying_saucer);
				}
			}
		}
	}

	for (int i = 0; i < 3; i++) {
		if (alien_shots[i].game_object.base.is_visible) {
			update(&alien_shots[i].game_object.base);
			animate(&alien_shots[i].game_object);
			
			if (alien_shots[i].game_object.base.is_enabled) {
				alien_shots[i].reload_ticks++;
			}
		}

		if (collides(&alien_shots[i].game_object.base, &player.game_object.base)) {
			alien_shots[i].game_object.base.is_visible = 0;
			alien_shots[i].game_object.base.is_enabled = 0;
			// alien_shots[i].reload_ticks = 0;

			player_lose_life(&player);
			player_explode_trans_ticks = 6 * 10;
			game_paused = 1;
		}
		else if (collides(&alien_shots[i].game_object.base, &bottom_bound)) {
			shot_explode(&alien_shots[i]);
			// alien_shots[i].reload_ticks = 0;
		} else if (collides(&alien_shots[i].game_object.base, &player_shot.game_object.base)) {
			shot_explode(&alien_shots[i]);
			// alien_shots[i].reload_ticks = 0;		

			player_shot.game_object.base.is_enabled = 0;
			player_shot.game_object.base.is_visible = 0;
		}
		else {
			for (int j = 0; j < SHIELDS_COUNT; j++) {
				if (pixel_collides(&shields[j], &alien_shots[i])) {
					shot_explode(&alien_shots[i]);
					// alien_shots[i].reload_ticks = 0;
				}
			}
		}
	}

	if (flying_saucer.game_object.base.is_visible) {
		update(&flying_saucer.game_object.base);
		
		if (flying_saucer.game_object.base.move_direction_x == MOVE_RIGHT_DIRECTION &&
			collides(&flying_saucer.game_object.base, &right_bound)
		) {
			flying_saucer.game_object.base.is_enabled = 0;
			flying_saucer.game_object.base.is_visible = 0;
		}
		else if (
			flying_saucer.game_object.base.move_direction_x == MOVE_LEFT_DIRECTION &&
			collides(&flying_saucer.game_object.base, &left_bound)
		){
			flying_saucer.game_object.base.is_enabled = 0;
			flying_saucer.game_object.base.is_visible = 0;
		}
	} else {
		--flying_saucer.ticks_count;
	}
	
	sprintf(score_label, "%04i", player.score);

	return game_id();
}

static void game_draw(void) {
	draw_background(context);
	draw_header(context, score_label, hiscore_label);
	draw_footer(context, &spritesheet, player.lives, PLAYER_WIDTH, PLAYER_HEIGHT);

	for (int i = 0; i < SHIELDS_COUNT; i++) {
		draw(context, &shields[i].game_object);
	}

	if (!aliens_spawn_drawn) {
		for (int i = 0; i < aliens_spawn_draw_index; i++) {
			draw(context, &aliens[i].game_object);
		}

		++aliens_spawn_draw_index;
	} else {
		for (int i = 0; i < ALIENS_COUNT; i++) {
			draw(context, &aliens[i].game_object);
		}
	}

	for (int i = 0; i < 3; i++) {
		draw(context, &alien_shots[i].game_object);
	}

	draw(context, &player.game_object);
	draw(context, &player_shot.game_object);

	if (flying_saucer.game_object.base.is_visible) {
		draw(context, &flying_saucer.game_object);
	}

	if (game_over) {
		draw_game_over(context);
	}

	window_present(window);
}

static void game_deinit(void) {
	// TODO: Save player hi-score to file
	if (player.score > player.hiscore) {
		sprintf(hiscore_label, "%04i", player.hiscore);
	}
}

void create_new_game(GameState *state, window_t *win, context_t *ctx) {
	state->id = game_id;
	state->init = game_init;
	state->update = game_update;
	state->draw = game_draw;
	state->deinit = game_deinit;

	window = win;
	context = ctx;
}

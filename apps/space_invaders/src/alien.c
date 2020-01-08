#include <stdlib.h>
#include "alien.h"

static FrameClip animation_frames[7];

static void init_animation_frames(void) {
	animation_frames[0] = (FrameClip) { 106, 0, 16, 16 };
	animation_frames[1] = (FrameClip) { 130, 0, 16, 16 };

	animation_frames[2] = (FrameClip) { 154, 0, 22, 16 };
	animation_frames[3] = (FrameClip) { 183, 0, 22, 16 };

	animation_frames[4] = (FrameClip) { 213, 0, 24, 16 };
	animation_frames[5] = (FrameClip) { 244, 0, 24, 16 };

	animation_frames[6] = (FrameClip) { 192, 22, 26, 16 };
}

static void alien_add(Alien *aliens, bmp_image_t *spritesheet, int row, int column, int x, int y) {
	GameObject base = GameObject(x, y, 16, 16, 5, 1, 0, 1, 1, 0, 25, 0);
	AnimatedGameObject game_object = AnimatedGameObject(base, spritesheet, animation_frames, 0, 0, 0, 0, 0, 0);
	Alien alien = (Alien) { game_object, 0, 0 };
	
	switch (row) {
		case 0:
			alien.points = 30;
			alien.type = 0;
			alien.game_object.base.width = 16;
			alien.game_object.base.x += 4;
			set_animation_state(&alien.game_object, 0, 1, 1, 25);
			break;
		case 1:
		case 2:
			alien.points = 20;
			alien.type = 1;
			alien.game_object.base.width = 22;
			alien.game_object.base.x += 3;
			set_animation_state(&alien.game_object, 2, 3, 1, 25);
			break;
		case 3:
		case 4:
			alien.points = 10;
			alien.type = 2;
			alien.game_object.base.width = 24;
			set_animation_state(&alien.game_object, 4, 5, 1, 25);
			break;
	}

	aliens[ALIEN_COLUMNS_COUNT * row + column] = alien;
}

void aliens_init(Alien *aliens, bmp_image_t *spritesheet, int start_y) {
	init_animation_frames();

	for (int i = 0; i < ALIEN_ROWS_COUNT; i++) {
		for (int j = 0; j < ALIEN_COLUMNS_COUNT; j++) {
			int x = ALIEN_DISTANCE_X * (j+1) + 16 * j;
            int y = start_y + (ALIEN_DISTANCE_Y + 16) * i;

			alien_add(aliens, spritesheet, i, j, x, y);
		}
	}
}

void alien_explode(Alien *alien) {
	set_animation_state(&alien->game_object, 6, 6, 0, 0);
	set_disappear_ticks_count(&alien->game_object, 5);

	alien->game_object.base.move_direction_x = 0;
	alien->game_object.base.move_direction_y = 0;
	alien->game_object.base.is_enabled = 0;
}

int alien_update_move_speed(Alien *aliens, int aliens_alive_count) {
	int move_ticks = -1;
	
	switch (aliens_alive_count) {
		case 44:
			move_ticks = 20;
			break;
		case 33:
		case 22:
			move_ticks = 15;
			break;
		case 11:
			move_ticks = 10;
			break;
		case 5:
			move_ticks = 5;
			break;
		case 1:
			move_ticks = 1;
			break;
	}

	if (move_ticks > -1) {
		for (int i = 0; i < ALIENS_COUNT; i++) {
			aliens[i].game_object.base.move_ticks_limit = move_ticks;
			aliens[i].game_object.draw_ticks_limit = move_ticks;
		}

		return 1;
	}

	return 0;
}

int get_alien_spawn_y(int spawns_count) {
	int spawn_y = ALIEN_INITIAL_SPAWN_Y;

	if (spawns_count > 1) {
		spawn_y += 30;
	}
	if (spawns_count > 2) {
		spawn_y += 30;
	}
	if (spawns_count > 3) {
		spawn_y += 20;
	}
    if (spawns_count > 3) {
		spawn_y += 20;
	}

	return spawn_y;
}

int alien_get_reload_ticks_count(int score) {
	// Adapt when timer is implemented
	if (score <= 200) {
      return 60;
    } else if (score <= 1000) {
      return 50;
    } else if (score <= 2000) {
      return 45;
    } else if (score <= 3000) {
      return 40;
    } else {
      return 30;
    }
}

int alien_can_fire(Shot *shots, int shot_index, int current_score) {
	if (shots[shot_index].game_object.base.is_visible) {
		return 0;
	}
	
	for (int i = 0; i < 3; i++) {
		int ticks = shots[i].reload_ticks;

		if (i != shot_index && ticks > 0 && ticks < alien_get_reload_ticks_count(current_score)) {
			return 0;
		}
    }

	return 1;
}

int alien_fire_shot(Alien *aliens, Shot *shots, int shot_index, int player_x, int current_score) {
	if (!alien_can_fire(shots, shot_index, current_score)) {
		return 0;
	}

	int index_choice = -1;

	if (shots[shot_index].type == ROLLING_SHOT) {
		// Alien rolling-shot (targets player specifically)
		int distance = -1;

		// Find alien nearest to the player.
		for(int i = 0; i < ALIEN_COLUMNS_COUNT; i++) {
			int new_distance = abs(player_x - aliens[i].game_object.base.x);
			if (distance != -1 && new_distance > distance) {
				break;
			}

			for (int j = ALIEN_ROWS_COUNT - 1; j >= 0; j--) {
				int index = (j * ALIEN_COLUMNS_COUNT) + i;

				if (aliens[index].game_object.base.is_visible) {
					index_choice = index;
					distance = new_distance;
					break;
				}
			}
		}
	} else {
		int column_index = rand_range(0, ALIEN_COLUMNS_COUNT - 1);

		for (int j = ALIEN_ROWS_COUNT - 1; j >= 0; j--) {
			int index = (j * ALIEN_COLUMNS_COUNT) + column_index;

			if (aliens[index].game_object.base.is_visible) {
				index_choice = index;
				break;
			}
		}
	}

	if (index_choice >= 0 && index_choice < ALIEN_ROWS_COUNT * ALIEN_COLUMNS_COUNT) {
		GameObject alien_body = aliens[index_choice].game_object.base;
		int x = alien_body.x + alien_body.width / 2;
		int y = alien_body.y + alien_body.height;

		shot_fire(&shots[shot_index], x, y, 1);
		shots[shot_index].reload_ticks = 1;
		
		return 1;
	}

	return 0;
}

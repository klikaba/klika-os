#ifndef __ALIEN_H
#define __ALIEN_H

#define ALIENS_COUNT 55
#define ALIEN_ROWS_COUNT 		  5
#define ALIEN_COLUMNS_COUNT 	 11
#define ALIEN_MOVE_DISTANCE_X 	  5
#define ALIEN_MOVE_DISTANCE_Y    20
#define ALIEN_DISTANCE_X 		 15
#define ALIEN_DISTANCE_Y 		 12
#define ALIEN_INITIAL_SPAWN_Y	137

#include "animated_game_object.h"
#include "shot.h"

typedef struct {
	AnimatedGameObject game_object;
	int points;
	int type;
} Alien;

void aliens_init(Alien *aliens, bmp_image_t *spritesheet, int start_y);
void alien_explode(Alien *alien);
int alien_fire_shot(Alien *aliens, Shot *shots, int shot_index, int player_x, int current_score);
int alien_update_move_speed(Alien *aliens, int aliens_alive_count);
int alien_get_reload_ticks_count(int score);
int get_alien_spawn_y(int spawns_count);

#endif

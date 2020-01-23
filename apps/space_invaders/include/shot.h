#ifndef __SHOT_H
#define __SHOT_H

#include "animated_game_object.h"

#define SHOTS_ARRAY_MAX_SIZE	3

typedef enum { 
	PLAYER_SHOT,
	ROLLING_SHOT,
	PLUNGER_SHOT,
	SQUIGGLY_SHOT
} ShotType;

typedef struct {
	AnimatedGameObject game_object;
	ShotType type;
	int reload_ticks;
} Shot;

void shot_init(Shot *shot, bmp_image_t *spritesheet, ShotType type);
void shot_fire(Shot *shot, int x, int y, int move_direction_y);
void shot_explode(Shot *shot);

#endif

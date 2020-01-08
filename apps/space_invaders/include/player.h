#ifndef __PLAYER_H
#define __PLAYER_H

#include "animated_game_object.h"

#define PLAYER_WIDTH           	  28
#define PLAYER_HEIGHT             17
#define PLAYER_SPAWN_X            20
#define PLAYER_SPAWN_Y           480
#define PLAYER_MOVE_SPEED		   6

typedef struct {
	AnimatedGameObject game_object;
	int lives;
	int score;
	int hiscore;
} Player;

void player_init(Player *player, bmp_image_t *spritesheet);
void player_lose_life(Player *player);
void player_reinit(Player *player);

#endif

#ifndef __SHIELD_H__
#define __SHIELD_H__

#define SHIELD_WIDTH       44
#define SHIELD_HEIGHT      32
#define SHIELDS_COUNT       4

#include "animated_game_object.h"
#include "shot.h"

typedef struct {
    AnimatedGameObject game_object;
    int active;
} Shield;

void shields_init(Shield *shields);
int pixel_collides(Shield *shield, Shot *shot);

#endif

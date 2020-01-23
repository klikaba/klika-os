#ifndef __FLYING_SAUCER_H
#define __FLYING_SAUCER_H

#define FLYING_SAUCER_WIDTH      32
#define FLYING_SAUCER_HEIGHT     14
#define FLYING_SAUCER_TICKS	   1800

#include "animated_game_object.h"

typedef struct {
	AnimatedGameObject game_object;
	int ticks_count;
} FlyingSaucer;

void flying_saucer_init(FlyingSaucer *flying_saucer, bmp_image_t *spritesheet);
void flying_saucer_spawn(FlyingSaucer *flying_saucer);
void flying_saucer_explode(FlyingSaucer *flying_saucer);
int flying_saucer_ready(FlyingSaucer *flying_saucer);
int flying_saucer_get_points(void);

#endif

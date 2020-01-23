#include <stdlib.h>
#include "flying_saucer.h"
#include "constants.h"

static int saucer_score_table[20] = { 50, 50, 100, 50, 150, 100, 150, 100, 300, 50, 150, 100, 300, 150, 100, 50, 150, 100, 300, 100 };

static FrameClip animation_frames[2];

static void init_animation_frames(void) {
	animation_frames[0] = (FrameClip) { 152, 43, 31, 14 };
	animation_frames[1] = (FrameClip) { 194, 43, 40, 14 };
}

void flying_saucer_init(FlyingSaucer *flying_saucer, bmp_image_t *spritesheet) {
    init_animation_frames();

	GameObject base = GameObject(0, 0, FLYING_SAUCER_WIDTH, FLYING_SAUCER_HEIGHT, 3, 1, 0, 0, 0, 0, 2, 0);
	AnimatedGameObject game_object = AnimatedGameObject(base, spritesheet, animation_frames, 30, 30, 30, 0, 0, 0);
	flying_saucer->game_object = game_object;
	flying_saucer->ticks_count = FLYING_SAUCER_TICKS;
}

void flying_saucer_spawn(FlyingSaucer *flying_saucer) {
    if (rand_range(1, 10) > 5) {
        flying_saucer->game_object.base.x = WINDOW_WIDTH - FLYING_SAUCER_WIDTH;
        flying_saucer->game_object.base.move_direction_x = MOVE_LEFT_DIRECTION;
    } else
    {
        flying_saucer->game_object.base.x = 0;
        flying_saucer->game_object.base.move_direction_x = MOVE_RIGHT_DIRECTION;
    }
    
    flying_saucer->game_object.base.y = 100;
	flying_saucer->game_object.base.is_enabled = 1;
    flying_saucer->game_object.base.is_visible = 1;
	flying_saucer->ticks_count = FLYING_SAUCER_TICKS;

    set_animation_state(&flying_saucer->game_object, 0, 0, 0, 0);
}

void flying_saucer_explode(FlyingSaucer *flying_saucer) {
    set_animation_state(&flying_saucer->game_object, 1, 1, 0, 5);
	set_disappear_ticks_count(&flying_saucer->game_object, 5);

    flying_saucer->game_object.base.move_direction_x = 0;
	flying_saucer->game_object.base.is_enabled = 0;
}

int flying_saucer_ready(FlyingSaucer *flying_saucer) {
    return !flying_saucer->game_object.base.is_visible && flying_saucer->ticks_count == 0;
}

int flying_saucer_get_points(void) {
    return saucer_score_table[rand_range(0, 20)];
}

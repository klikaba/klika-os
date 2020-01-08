#include "shot.h"

static FrameClip animation_frames[14];

static void init_animation_frames(void) {
	// Player shot
	animation_frames[0] = (FrameClip) { 52, 23, 4, 7 };

	// Plunger shot
	animation_frames[1] = (FrameClip) { 63, 23, 6, 11 };
	animation_frames[2] = (FrameClip) { 79, 23, 6, 11 };
	animation_frames[3] = (FrameClip) { 95, 23, 6, 11 };
	animation_frames[4] = (FrameClip) { 109, 23, 6, 11 };

	// Rolling shot
	animation_frames[5] = (FrameClip) { 51, 43, 6, 14 };
	animation_frames[6] = (FrameClip) { 63, 43, 6, 14 };
	animation_frames[7] = (FrameClip) { 79, 43, 6, 14 };
	
	// Squiggly shot
	animation_frames[8] = (FrameClip) { 95, 43, 6, 14 };
	animation_frames[9] = (FrameClip) { 109, 43, 6, 14 };
	animation_frames[10] = (FrameClip) { 123, 43, 6, 14 };
	animation_frames[11] = (FrameClip) { 137, 43, 6, 14 };

	// Player shot explode
	animation_frames[12] = (FrameClip) { 242, 40, 26, 17 };

	// Alien shot explode
	animation_frames[13] = (FrameClip) { 128, 21, 12, 16 };
}

void shot_init(Shot *shot, bmp_image_t *spritesheet, ShotType type) {
	init_animation_frames();

	GameObject base = GameObject(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	AnimatedGameObject game_object = AnimatedGameObject(base, spritesheet, animation_frames, 0, 0, 0, 0, 0, 0);

	shot->game_object = game_object;
	shot->reload_ticks = 0;
	shot->type = type;
}

void shot_fire(Shot *shot, int x, int y, int move_direction_y) {
	shot->game_object.base = (GameObject)GameObject(x, y, 0, 0, 0, 0, move_direction_y, 1, 1, 0, 1, 0);

	switch (shot->type) {
		case PLAYER_SHOT:
			set_animation_state(&shot->game_object, 0, 0, 0, 0);
			shot->game_object.base.width = 4;
			shot->game_object.base.height = 7;
			shot->game_object.base.move_speed = 9;
			shot->game_object.base.move_ticks_limit = 0;
			break;
		case PLUNGER_SHOT:
			set_animation_state(&shot->game_object, 1, 4, 1, 4);
			shot->game_object.base.width = 6;
			shot->game_object.base.height = 14;
			shot->game_object.base.move_speed = 7;
			break;
		case ROLLING_SHOT:
			set_animation_state(&shot->game_object, 5, 7, 1, 4);
			shot->game_object.base.width = 6;
			shot->game_object.base.height = 14;
			shot->game_object.base.move_speed = 7;
			break;
		case SQUIGGLY_SHOT:
			set_animation_state(&shot->game_object, 8, 11, 1, 4);
			shot->game_object.base.width = 6;
			shot->game_object.base.height = 14;
			shot->game_object.base.move_speed = 7;
			break;
	}
}

void shot_explode(Shot *shot) {
	if (shot->type == PLAYER_SHOT) {
		set_animation_state(&shot->game_object, 12, 12, 0, 0);
	} else {
		set_animation_state(&shot->game_object, 13, 13, 0, 0);
		shot->game_object.base.y -= 9;
	}
	
	set_disappear_ticks_count(&shot->game_object, 5);

	shot->game_object.base.move_direction_y = 0;

	shot->game_object.base.is_enabled = 0;
	shot->game_object.base.x -= shot->game_object.sprite_frames[shot->game_object.current_frame].w / 2;
}

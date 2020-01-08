#include "player.h"

static FrameClip animation_frames[3];

static void init_animation_frames(void) {
	animation_frames[0] = (FrameClip) { 0, 0, 28, 17 };
	animation_frames[1] = (FrameClip) { 31, 0, 27, 17 };
	animation_frames[2] = (FrameClip) { 61, 0, 37, 17 };
}

void player_init(Player *player, bmp_image_t *spritesheet) {	
	init_animation_frames();

	GameObject base = GameObject(PLAYER_SPAWN_X, PLAYER_SPAWN_Y, PLAYER_WIDTH, PLAYER_HEIGHT, PLAYER_MOVE_SPEED, 0, 0, 1, 1, 0, 0, 0);
	AnimatedGameObject game_object = AnimatedGameObject(base, spritesheet, animation_frames, 0, 0, 0, 0, 0, 0);
	set_animation_state(&player->game_object, 0, 0, 0, 0);

	player->game_object = game_object;
	player->lives = 3;
	player->score = 0;
	player->hiscore = 0;
}

void player_lose_life(Player *player) {
	set_animation_state(&player->game_object, 1, 2, 1, 6);
	set_disappear_ticks_count(&player->game_object, 6 * 8);

	player->game_object.base.move_direction_x = 0;
	player->game_object.base.is_enabled = 0;
	player->lives--;
}

void player_reinit(Player *player) {
	player->game_object.base.x = PLAYER_SPAWN_X;
	player->game_object.base.is_enabled = 1;
	player->game_object.base.is_visible = 1;
	player->game_object.base.move_direction_x = 0;

	set_animation_state(&player->game_object, 0, 0, 0, 0);
}

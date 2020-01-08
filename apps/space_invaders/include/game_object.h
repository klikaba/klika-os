#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

typedef struct {
    int x;
	int y;
	int width;
	int height;
	int move_speed;
	int move_direction_x;
	int move_direction_y;
	int is_enabled;
	int is_visible;
	int move_ticks_count;
	int move_ticks_limit;
	int disappear_ticks_count;
} GameObject;

#define GameObject(x, y, width, height, move_speed, move_direction_x, move_direction_y, is_enabled, is_visible, move_ticks_count, move_ticks_limit, disappear_ticks_count) \
	{ x, y, width, height, move_speed, move_direction_x, move_direction_y, is_enabled, is_visible, move_ticks_count, move_ticks_limit, disappear_ticks_count }

// void init(GameObject *game_object);
int update(GameObject *game_object);
int collides(GameObject *object_1, GameObject *object_2);

#endif

#include "game_object.h"

int update(GameObject *game_object) {
    if (game_object->disappear_ticks_count > 0) {
        --game_object->disappear_ticks_count;

        if (game_object->disappear_ticks_count == 0) {
            game_object->is_enabled = 0;
            game_object->is_visible = 0;
        }
    }

    if (game_object->move_ticks_count++ >= game_object->move_ticks_limit) {
        // TODO: Multiply with dt when timer is implemented
        game_object->x += game_object->move_direction_x * game_object->move_speed;
        game_object->y += game_object->move_direction_y * game_object->move_speed;

        game_object->move_ticks_count = 0;
        
        return 1;
    }

    return 0;
}

int collides(GameObject *object_1, GameObject *object_2) {
    return object_1->is_enabled && object_2->is_enabled &&
           object_1->x < object_2->x + object_2->width &&
           object_1->x + object_1->width > object_2->x &&
           object_1->y < object_2->y + object_2->height &&
           object_1->y + object_1->height > object_2->y;
}

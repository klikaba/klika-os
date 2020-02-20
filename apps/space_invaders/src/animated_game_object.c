#include "animated_game_object.h"

static void set_next_frame(AnimatedGameObject *game_object) {
    if (game_object->current_frame++ == game_object->end_frame) {
        game_object->current_frame = game_object->start_frame;
    }
}

void animate(AnimatedGameObject *game_object) {
    if (game_object->animation_active && game_object->base.is_visible) {
        if (game_object->draw_ticks_count++ >= game_object->draw_ticks_limit) {
            set_next_frame(game_object);
            
            game_object->draw_ticks_count = 0;
        }
    }
}

void draw(context_t *context, AnimatedGameObject *game_object) {
    if (game_object->base.is_visible) {
        FrameClip *frame = &game_object->sprite_frames[game_object->current_frame];
        bmp_blit_clipped(context, game_object->spritesheet, game_object->base.x, game_object->base.y, frame->x, frame->y, frame->w, frame->h);
    }
}

void set_animation_state(AnimatedGameObject *game_object, int start_frame, int end_frame, int animation_active, int draw_ticks_limit) {
    game_object->start_frame = start_frame;
    game_object->current_frame = start_frame;
    game_object->end_frame = end_frame;

    game_object->draw_ticks_count = 0;
    game_object->draw_ticks_limit = draw_ticks_limit;
    game_object->animation_active = animation_active;
}

void set_disappear_ticks_count(AnimatedGameObject *game_object, int disappear_ticks_count) {
    game_object->base.disappear_ticks_count = disappear_ticks_count;
}

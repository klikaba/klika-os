#ifndef __ANIMATED_GAME_OBJECT_H__
#define __ANIMATED_GAME_OBJECT_H__

#include <windows.h>
#include <gfx.h>
#include <bmp.h>

#include "game_object.h"

typedef struct {
    int x, y, w, h;
} FrameClip;

typedef struct {
    GameObject base;
    bmp_image_t *spritesheet;
    FrameClip *sprite_frames;

    int start_frame;
    int current_frame;
    int end_frame;

    int draw_ticks_count;
    int draw_ticks_limit;

    int animation_active;
} AnimatedGameObject;

#define AnimatedGameObject(base, spritesheet, sprite_frames, start_frame, current_frame, end_frame, draw_ticks_count, draw_ticks_limit, animation_active) \
    { base, spritesheet, sprite_frames, start_frame, current_frame, end_frame, draw_ticks_count, draw_ticks_limit, animation_active }

// void init(AnimatedGameObject *game_object);
void animate(AnimatedGameObject *game_object);
void draw(context_t *context, AnimatedGameObject *game_object);

void set_animation_state(AnimatedGameObject *game_object, int start_frame, int end_frame, int animation_active, int draw_ticks_limit);
void set_disappear_ticks_count(AnimatedGameObject *game_object, int disappear_ticks_count);

#endif

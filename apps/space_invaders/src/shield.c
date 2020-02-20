#include "shield.h"
#include "constants.h"

static FrameClip animation_frames[1];

static bmp_image_t shield_sprites[SHIELDS_COUNT];
static bmp_image_t shot_explode_top;
static bmp_image_t shot_explode_bottom;

static void init_animation_frames(void) {
	animation_frames[0] = (FrameClip) { 0, 0, 44, 32 };
}

static void shield_init(Shield *shield, int index) {
    int x = 26 + SHIELD_WIDTH * (index + 1) + SHIELD_WIDTH * index;
    int y = 419;

    GameObject base = GameObject(x, y, SHIELD_WIDTH, SHIELD_HEIGHT, 0, 0, 0, 1, 1, 0, 0, 0);
    AnimatedGameObject game_object = AnimatedGameObject(base, &shield_sprites[index], animation_frames, 0, 0, 0, 0, 0, 0);
    shield->game_object = game_object;
    shield->active = 1;
}

void shields_init(Shield *shields) {
    init_animation_frames();
    bmp_from_file("/apps/invaders/00.bmp", &shot_explode_bottom);
    bmp_from_file("/apps/invaders/01.bmp", &shot_explode_top);

    for (int i = 0; i < SHIELDS_COUNT; i++) {
        bmp_from_file("/apps/invaders/shield.bmp", &shield_sprites[i]);
        shield_init(&shields[i], i);
    }
}

static void bmp_put_data(bmp_image_t *bmp_dest, bmp_image_t *bmp_src, int x, int y) {
	int dest_width = bmp_dest->header->width_px;
    int dest_height = bmp_dest->header->height_px;
    int dest_start_x = x;
    int dest_start_y = y;
    
    int src_width = bmp_src->header->width_px;
    int src_height = bmp_src->header->height_px;
    int src_start_x = 0;
    int src_start_y = 0;

    if (x - src_width/2 < 0) {
        src_start_x += src_width/2 - x;
        dest_start_x = 0;
    } else {
        dest_start_x -= src_width/2;
    }
    
    uint32_t *dest = bmp_dest->data + dest_width * dest_start_y + dest_start_x;
	uint32_t *src = bmp_src->data + src_width * src_start_y + src_start_x;

	for (int i=0; i < src_height - src_start_y; i++) {
		for (int j=0; j < src_width - src_start_x; j++) {
            if (dest_start_x + j == dest_width) {
                src += (src_width - j);
                dest += (src_width - j);
                break;
            }

			if (*src != 4278190080){
				*dest = *src;
            }

            src++;
            dest++;
		}

        if (dest_start_y + i == dest_height) break;

        src += src_start_x;
		dest += bmp_dest->header->width_px - (src_width - src_start_x);
	}
}

static void erase_white_pixels(bmp_image_t *bmp) {
    uint32_t *dest = bmp->data;
    int height = bmp->header->height_px;
	int width = bmp->header->width_px;

    for (int i=0; i<height; i++) {
		for (int j=0; j<width; j++) {
			if (*dest == 4294967295)
				*dest = 4278190080;
            dest++;
		}
	}
}

static int pixel_overlap(GameObject *game_object, int x, int y) {
    return !(
        x < game_object->x || x > game_object->x + game_object->width ||
        y < game_object->y || y > game_object->y + game_object->height
    );
}

int pixel_collides(Shield *shield, Shot *shot) {
    if (!shield->active)
        return 0;

    if (collides(&shield->game_object.base, &shot->game_object.base)) {
        uint32_t *data = shield->game_object.spritesheet->data;

        int x = shield->game_object.base.x;
        int y = shield->game_object.base.y;

        if (shot->game_object.base.move_direction_y == MOVE_UP_DIRECTION) {
            for (int i = SHIELD_HEIGHT - 1; i >= 0; i--) {
                for (int j = 0; j < SHIELD_WIDTH; j++) {                
                    if (*data == 4280352544 && pixel_overlap(&shot->game_object.base, x + j, y + i)) {
                        bmp_put_data(shield->game_object.spritesheet, &shot_explode_bottom, j, SHIELD_HEIGHT - 1 - i);
                        erase_white_pixels(shield->game_object.spritesheet);
                        return 1;
                    }

                    data++;
                }
            }
        } else if (shot->game_object.base.move_direction_y == MOVE_DOWN_DIRECTION) {
            data += SHIELD_HEIGHT * SHIELD_WIDTH;

             for (int i = 0 ; i < SHIELD_HEIGHT; i++) {
                for (int j = SHIELD_WIDTH - 1; j >= 0; j--) {
                    if (*data == 4280352544 && pixel_overlap(&shot->game_object.base, x + j, y + i)) {
                        bmp_put_data(shield->game_object.spritesheet, &shot_explode_top, j, SHIELD_HEIGHT - 11 - i);
                        erase_white_pixels(shield->game_object.spritesheet);
                        return 1;
                    }

                    data--;
                }
            }
        }
    }

    return 0;
}

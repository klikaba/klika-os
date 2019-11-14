#ifndef __WINDOW_MANAGER_H
#define __WINDOW_MANAGER_H

#include <stdint.h>

#define SCREEN_UPDATE_TRESHOLD (1193180 / 1000)

typedef struct window_struct {
	uint32_t handle;
	int x;
	int y;
	int z;
	int width;
	int height;
} window_t;

#define MAX_WINDOW_COUNT 64

extern window_t* window_list[MAX_WINDOW_COUNT];

window_t* window_create(int x, int y, int width, int height);
void init_kernel_window_manager();
void window_manager_redraw();

#endif
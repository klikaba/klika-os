#ifndef __WINDOW_MANAGER_H
#define __WINDOW_MANAGER_H

#include <messaging.h>
#include <process.h>
#include <stdbool.h>
#include <stdint.h>
#include <mouse.h>

#define MAX_WINDOW_NAME_LENGTH 128
#define MAX_MESSAGE_QUEUE_LENGTH 128

typedef struct window_struct {
	uint32_t handle;
	int x;
	int y;
	int z;
	int width;
	int height;
	char title[MAX_WINDOW_NAME_LENGTH];
	int message_queue_index;
	task_t* parent_task;
	message_t message_queue[MAX_MESSAGE_QUEUE_LENGTH];
} window_t;

#define MAX_WINDOW_COUNT 64

extern window_t* window_list[MAX_WINDOW_COUNT];

window_t* window_create(int x, int y, int width, int height, char* title);
void window_add_message(message_t msg, window_t* win);
void window_add_messageto_top(message_t msg);
bool window_pop_message(message_t* msg_out, window_t* win);
window_t* window_find(uint32_t handle);
void init_kernel_window_manager();
void window_manager_redraw();

#endif
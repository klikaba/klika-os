#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef uint32_t window_handle;

#define WINDOW_EXT(win) ((window_ext_t*)win->ext)

#define WINDOW_STATE_CREATED 0
#define WINDOW_STATE_DESTORY 1

#define WINDOW_LIB_MESSAGE_CREATE 	10000
#define WINDOW_LIB_MESSAGE_PREDRAW	10001
#define WINDOW_LIB_MESSAGE_DRAW   	10002
#define WINDOW_LIB_MESSAGE_PRESENT 	10003

#define WINDOW_MAX_COMPONENTS 32

#define WIN_BACKGROUND_COLOR 	0x7F7F7F
#define WIN_FRAME_COLOR 			0xFFFFFF
#define WIN_INACTIVE_FRAME  	0x0000AA
#define WIN_ACTIVE_FRAME    	0x0000FF
#define WIN_FRAME_TEXT_COLOR  0xFFFFFF

#define WINDOW_BAR_HEIGHT  (8 + 2*3)

struct window_struct;
struct message_struct;
typedef bool (*window_procedure_t)(struct window_struct *, struct message_struct *);

typedef struct {
	uint32_t* buffer;
	int width;
	int height;
	int bpp;
} context_t;

typedef struct {
	context_t *context;
} window_ext_t;

struct window_struct {
	// Basic params
	window_handle handle;
	int x, y;
	int width, height;
	int state;
	char* title;

	// Win proc
	window_procedure_t window_procedure;
	window_procedure_t window_default_procedure;

	// Extended parameters (type dictates)
	void *ext;
};

typedef struct window_struct window_t;

window_t *window_create(int x, int y, int width, int height, char* title);
bool window_default_procedure(window_t *win, struct message_struct *msg);

void window_change_state(window_t* win, int state);

void on_window_predraw(window_t *win);
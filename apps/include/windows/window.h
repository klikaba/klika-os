#pragma once

#include <stdbool.h>
#include <stdint.h>

#define WINDOW_TYPE_WINDOW 0

#define WINDOW_EXT(win) ((window_ext_t*)win->ext)

#define WINDOW_STATE_CREATED 0
#define WINDOW_STATE_DESTORY 1

#define WINDOW_LIB_MESSAGE_CREATE 	1000
#define WINDOW_LIB_MESSAGE_PREDRAW	1001
#define WINDOW_LIB_MESSAGE_DRAW   	1002
#define WINDOW_LIB_MESSAGE_PRESENT 	1003

#define WINDOW_USER_MESSAGE         10000

#define WINDOW_MAX_COMPONENTS 32

#define WIN_BACKGROUND_COLOR 	0x7F7F7F
#define WIN_FRAME_COLOR 			0xFFFFFF
#define WIN_INACTIVE_FRAME  	0x0000AA
#define WIN_ACTIVE_FRAME    	0x0000FF
#define WIN_FRAME_TEXT_COLOR  0xFFFFFF

#define WINDOW_BAR_HEIGHT  (8 + 2*3)

#define TEXT_FONT_WIDTH(str) (strlen(str)*8)
#define TEXT_FONT_HEIGHT(str) (8)

typedef uint32_t window_handle;

struct window_struct;
struct message_struct;
typedef bool (*window_procedure_t)(struct window_struct *, struct message_struct *);

typedef struct {
	int x;
	int y;
} vector_t;

typedef struct {
	uint32_t* buffer;
	int width;
	int height;
	int bpp;
} context_t;

typedef struct {
	context_t *context;
	struct window_struct *focused;
} window_ext_t;

struct window_struct {
	// Basic params
	window_handle handle;
	int id;
	int type;
	int x, y;
	int width, height;
	int state;
	char* title;

	// Win proc
	window_procedure_t window_procedure;
	window_procedure_t default_procedure;

	// Extended parameters (based on type ... check TYPE_EXT macro e.g. WINDOW_EXT)
	void *ext;

	// Linked list (in fact it should be general tree to have proper Z order)
	struct window_struct *next;
	struct window_struct *parent;
};

typedef struct window_struct window_t;

window_t *window_create(int x, int y, int width, int height, char* title, int id);
bool window_default_procedure(window_t *win, struct message_struct *msg);
void window_add_child(window_t *parent, window_t *child);

void window_dispatch(window_t *win, struct message_struct *msg);
bool window_dispatch_message(window_t *win, struct message_struct *msg);
bool window_dispatch_message_simple(window_t *win, int message);

void window_invalidate(window_t *win);
void window_change_state(window_t* win, int state);
void on_window_predraw(window_t *win);

bool window_point_inside(window_t* win, int x, int y);
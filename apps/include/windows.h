#ifndef __WINDOWS_H
#define __WINDOWS_H

#include <messaging.h>
#include <stdbool.h>
#include <stdint.h>

typedef uint32_t window_handle;

#define WINDOW_STATE_CREATED 0
#define WINDOW_STATE_DESTORY 1

#define WINDOW_LIB_MESSAGE_CREATE 	10000
#define WINDOW_LIB_MESSAGE_PREDRAW	10001
#define WINDOW_LIB_MESSAGE_DRAW   	10002
#define WINDOW_LIB_MESSAGE_PRESENT 	10003

#define WIN_BACKGROUND_COLOR 	0x7F7F7F
#define WIN_FRAME_COLOR 			0xFFFFFF
#define WIN_INACTIVE_FRAME  	0x0000AA
#define WIN_ACTIVE_FRAME    	0x0000FF
#define WIN_FRAME_TEXT_COLOR  0xFFFFFF

#define WINDOW_BAR_HEIGHT  (8 + 2*3)

struct window_struct;
typedef bool (*window_procedure_t)(message_t *, struct window_struct *);

typedef struct {
	uint32_t* buffer;
	int width;
	int height;
	int bpp;
} context_t;

struct window_struct {
	window_handle handle;
	window_procedure_t window_procedure;
	window_procedure_t default_window_procedure;
	bool redraw;
	int state;
	context_t context;
	char* title;
};
typedef struct window_struct window_t;

/////////////////// MESSAGING /////////////////////////
bool window_get_message(message_t* msg, window_t* win);
void window_send_message(message_t* msg, window_t* win);
void window_send_message_simple(int message, window_t* win);


/////////////////// WINDOW PROCEDURE /////////////////////////
void window_dispatch_message(message_t* msg, window_t* win);
void window_predraw(window_t *win);
bool window_procedure(message_t* msg, struct window_struct* win);

bool default_window_procedure(message_t* msg, struct window_struct* win);

///////////////////// WINDOW ACTIONS ///////////////////////
window_t window_create(int x, int y, int width, int height, char* title);
void window_change_state(window_t* win, int state);
void window_predraw(window_t *win);

#endif
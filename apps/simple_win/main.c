#include <messaging.h>
#include <klika_os.h>
#include <syscalls.h>
#include <malloc.h>
#include <stdlib.h>

typedef struct {
	uint32_t* buffer;
	int width;
	int height;
} context_t;

typedef struct {
	context_t* context;
	long handle;
} window_t;

context_t* context_create(int width, int height, int bpp) {
	//assert(bpp == 32);
	context_t* context = (context_t*)malloc(sizeof(context_t));
	context->buffer = (uint32_t*)malloc(width * height * (bpp / 8)); // XRGB for now only
	return context;
}

window_t windows_init(int x, int y, int width, int height, char* title) {
	window_t win;

	win.context = context_create(width, height, 32);
	win.handle = syscall(SYSCall_windows_create, x, y, width, height, title);
	DEBUG("Win handle: %i", win.handle);
	return win;
}

#include <font_8x8.h>

#define FRAMEBUFFER_32 (context->buffer)

#define FIRST_PIXEL(x, y) ((x) + ((y) * (context->width)))

// Be carefull with this macro :)
#define CLIP_X(x) x = x < 0 ? 0 : x; x = x < context->width ? x : context->width-1
#define CLIP_Y(y) y = y < 0 ? 0 : y; y = y < context->height ? y : context->height-1
#define CLIP_XY(x, y) CLIP_X(x); CLIP_Y(y)

void context_putpixel(context_t* context, int x, int y, uint32_t color) {
	CLIP_XY(x, y);
	FRAMEBUFFER_32[FIRST_PIXEL(x, y)] = color;
}

void context_hline(context_t* context, int x1, int x2, int y, uint32_t color) {
	CLIP_X(x1);
	CLIP_X(x2);
	CLIP_Y(y);
	for (int x=x1; x<=x2; x++) {
		FRAMEBUFFER_32[FIRST_PIXEL(x, y)] = color;
	}
}

void context_fillrect(context_t* context, int x1, int y1, int x2, int y2, uint32_t color) {
	for (int i=y1; i<=y2; i++) {
		context_hline(context, x1, x2, i, color);
	}
}

void context_putchar(context_t* context, int x, int y, uint32_t fgcolor, uint32_t bgcolor, const char c) {
  uint8_t i, j;
  for(i = 0; i < 8; i++) {
    for(j = 0; j < 8; j++) {
    	if (x+i >= 0 && x+1 < context->width &&
    		  y+j >= 0 && y+1 < context->height) {
  			FRAMEBUFFER_32[FIRST_PIXEL(x+i, y+j)] = ((font8x8_basic[c & 0x7F][j] >> i ) & 1) ? fgcolor : bgcolor;
  		}
    }
  }
}

void context_puts(context_t* context, int x, int y, uint32_t fgcolor, uint32_t bgcolor, const char *c) {
	while(*c) {
		context_putchar(context, x, y, fgcolor, bgcolor, *c++);
		x += 8;
	}
}

window_t window;
message_t msg;

int main() {
	DEBUG("App started :%i", 10);
	
	window = windows_init(100, 100, 600, 400, "Hello app");

	context_puts(window.context, 20, 10, 0, 0xCCCCCC, "Version 1.0");

	while(syscall(SYSCall_windows_get_message, &msg, window.handle)) { 
		if (msg.message != 0) {
			DEBUG("Got message: %i", msg.message);
		}
	}
	/*
	while(get_message(&msg)) {
		dispatch(&msg);
	}
	*/
	return 0;
}
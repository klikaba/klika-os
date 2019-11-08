#ifndef __CONSOLE_H
#define __CONSOLE_H

#include <kernel.h>
#include <memory.h>

#define CONSOLE_80_25_SIZE (80*25*2)
#define CONSOLE_BG_FG_COLOR(fg_color, bg_color) ((bg_color << 4) | (fg_color & 0x0F))
#define CONSOLE_VIDEO_MEMORY ((char*)0xFFFF800000000000 + 0xB8000)

enum COLORS {
    BLACK,
    BLUE,
    GREEN,
    CYAN,
    RED,
    MAGENTA,
    BROWN,
    LIGHTGRAY,
    DARKGRAY,       
    LIGHTBLUE,
    LIGHTGREEN,
    LIGHTCYAN,
    LIGHTRED,
    LIGHTMAGENTA,
    YELLOW,
    WHITE
};

typedef struct {
	int id;
	char color;
	char *buffer;
	uint16_t  current_index;
} console_t; 

extern int __CONSOLE_ID;

console_t* console_init(console_t *console);
console_t* console_reset(console_t *console);
void console_putch(console_t *console, char ch);
void console_puts(console_t *console, char *string);

int cprintf_help(unsigned c, void *ptr);
void console_printf(console_t *console, const char *fmt, ...);

void move_cursor(uint16_t pos);

#endif

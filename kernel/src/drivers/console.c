#include <console.h>
#include <string.h>
#include <stdarg.h>
#include <_printf.h>

int __console_id = 0;

console_t* console_init(console_t *console) {
	console_reset(console);
	return console;
}

console_t* console_reset(console_t *console) {
	console->id = __console_id++;
	console->color = CONSOLE_BG_FG_COLOR(LIGHTGRAY, BLACK);
	console->current_index = 0;
	memset(console->buffer, 0, CONSOLE_80_25_SIZE);
	return console;
}

void console_putch(console_t *console, char ch) {
	// char *buffer = console->buffer + console->current_index*2;

	switch(ch) {
		case '\n':  // backspace
			console->current_index = ((console->current_index + 80) / 80) * 80;
			break;
		case '\r':  // backspace
			break;
		default:
			console->buffer[console->current_index*2] = ch;
			console->buffer[console->current_index*2+1] = console->color;
			// *buffer++ = ch;
			// *buffer++ = console->color;
			console->current_index++;
			if (console->current_index >= 80*25) {
				console->current_index = 0;
			}
			break;
	}
}

void console_puts(console_t *console, char *string) {
	while(*string) {
		console_putch(console, *string++);
	}
	console_putch(console, '\n');
}


int cprintf_help(unsigned c, void *ptr) {
	console_t *console = (console_t*)ptr;
	console_putch(console, c);
	return 0;
}

void console_printf(console_t *console, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	(void)_printf(fmt, args, cprintf_help, console);
	va_end(args);
}

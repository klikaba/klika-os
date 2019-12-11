#include <klikaos.h>
#include <windows.h>
#include <stdlib.h>
#include <gfx.h>

message_t msg;
window_t  *window;
context_t *context;

#define MSG_USER_WIN (WINDOW_USER_MESSAGE + 1)

#define RGBA(r,g,b) (((r) << 16) | ((g) << 8) | ((b) & 0xFF))

void draw_frame() {
	int x1 = rand() % window->width;
	int x2 = rand() % window->width;
	int y1 = rand() % (window->height - WINDOW_BAR_HEIGHT) + WINDOW_BAR_HEIGHT;
	int y2 = rand() % (window->height - WINDOW_BAR_HEIGHT) + WINDOW_BAR_HEIGHT;

	uint32_t col = RGBA(rand()%255, rand()%255, rand()%255);

  gfx_line(context, x1, y1, x2, y2, col );
	window_present(window);
}

int main() {
	window = window_create(rand() % 700, rand() % 400, 300, 300, "Demo graphics", MSG_USER_WIN, WINDOW_ATTR_NONE, WINDOW_FRAME_TRANSPARENT);
	context = WINDOW_EXT(window)->context;

	while(1) {
		window_peek_message(window, &msg);
		if (msg.message != 0) {
			window_dispatch(window, &msg);
		}
		draw_frame();
	}
	return 0;
}

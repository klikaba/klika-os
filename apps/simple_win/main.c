#include <klika_os.h>
#include <windows.h>
#include <stdlib.h>
#include <gfx.h>

message_t msg;
window_t window;
long counter = 0;
char buff[123];

void do_app_draw() {
	sprintf(buff, "Count: %i", counter++);

	gfx_puts(&window.context, 10, 20, 0, WIN_BACKGROUND_COLOR, buff);

	window_send_message_simple(WINDOW_LIB_MESSAGE_PREDRAW, &window);
}


int main() {
	window = window_create(100, 100, 600, 400, "Hello World ;)");

	DEBUG("Win context: 0x%X", window.context.buffer);
	while(window_get_message(&msg, &window)) { 
		if (msg.message != 0) {
			switch(msg.message) {
				case MESSAGE_MOUSE_MOVE:
					// DEBUG("Message: %i - Mouse move (%i, %i)", msg.message, msg.mouse_x, msg.mouse_y);
					break;
				case MESSAGE_MOUSE_CLICK:
					// DEBUG("Message: %i - Mouse click (%i, %i) : %i", msg.message, msg.mouse_x, msg.mouse_y, msg.mouse_buttons);
					break;
				case MESSAGE_KEY_PRESS:
					// DEBUG("Message: %i - Key press (%c)", msg.message, msg.key & 0xFF);
					break;
				case MESSAGE_KEY_RELEASE:
					// DEBUG("Message: %i - Key release (%c)", msg.message, msg.key & 0xFF);
					break;
				case WINDOW_LIB_MESSAGE_DRAW:
					do_app_draw();
					break;
				default:
					// DEBUG("Got message : %i", msg.message);
					break;
			}
			window_dispatch_message(&msg, &window);
		}
	}
	return 0;
}
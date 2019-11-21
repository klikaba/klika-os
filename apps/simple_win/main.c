#include <klika_os.h>
#include <windows.h>
#include <stdlib.h>
#include <gfx.h>

message_t msg;
window_t  *window;
window_t  *button;
window_t  *label;

long counter = 0;

#define MSG_USER_WIN (WINDOW_USER_MESSAGE + 1)
#define MSG_USER_BTN (WINDOW_USER_MESSAGE + 2)
#define MSG_USER_LABEL (WINDOW_USER_MESSAGE + 3)

void increment_counter() {
	char buff[123];
	sprintf(buff, "Count: %i", counter++);

	label_set_text(label, buff);
}

int main() {
	window = window_create(100, 100, 300, 300, "Hello World ;)", MSG_USER_WIN);
	button = button_create(window, 10, 50, 100, 20, "Click me", MSG_USER_BTN);
	label = label_create(window, 10, 80, 100, 20, "", MSG_USER_LABEL);

	while(window_get_message(window, &msg)) { 
		switch(msg.message) {
			// case MESSAGE_MOUSE_MOVE:
			// 	// DEBUG("Message: %i - Mouse move (%i, %i)", msg.message, msg.mouse_x, msg.mouse_y);
			// 	break;
			// case MESSAGE_MOUSE_PRESS:
			// 	DEBUG("Message: %i - Mouse press (%i, %i) : %i", msg.message, msg.x, msg.y, msg.key);
			// 	break;
			// case MESSAGE_MOUSE_RELEASE:
			// 	DEBUG("Message: %i - Mouse release (%i, %i) : %i", msg.message, msg.x, msg.y, msg.key);
			// 	break;
			// case MESSAGE_KEY_PRESS:
			// 	// DEBUG("Message: %i - Key press (%c)", msg.message, msg.key & 0xFF);
			// 	break;
			// case MESSAGE_KEY_RELEASE:
			// 	// DEBUG("Message: %i - Key release (%c)", msg.message, msg.key & 0xFF);
			// 	break;
			// case WINDOW_LIB_MESSAGE_DRAW:
			// 	do_app_draw();
			// 	break;
			case MSG_USER_BTN:
				increment_counter();
				break;
			// default:
			// 	DEBUG("Got message : %i", msg.message);
			// 	break;
		}
		window_dispatch(window, &msg);
	}
	return 0;
}
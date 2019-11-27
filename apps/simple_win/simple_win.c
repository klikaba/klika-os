#include <klika_os.h>
#include <windows.h>
#include <stdlib.h>
#include <gfx.h>

message_t msg;
window_t  *window;
window_t  *label;

long counter = 0;

#define MSG_USER_WIN (WINDOW_USER_MESSAGE + 1)
#define MSG_USER_BTN1 (WINDOW_USER_MESSAGE + 2)
#define MSG_USER_BTN2 (WINDOW_USER_MESSAGE + 3)
#define MSG_USER_LABEL (WINDOW_USER_MESSAGE + 4)

void increment_counter(int add) {
	char buff[123];

	counter += add;
	sprintf(buff, "Count: %i", counter);
	label_set_text(label, buff);
}

int main() {
	int layout_y = WINDOW_BAR_HEIGHT + 10;
	window = window_create(100, 100, 300, 300, "Simple Window", MSG_USER_WIN);
	button_create(window, 10, layout_y, 100, 30, "Click me +", MSG_USER_BTN1);
	button_create(window, 120, layout_y, 100, 30, "Click me -", MSG_USER_BTN2);
	label = label_create(window, 10, layout_y + 40, 100, 20, "", MSG_USER_LABEL);

	while(window_get_message(window, &msg)) { 
		switch(msg.message) {
			case MSG_USER_BTN1:
				increment_counter(1);
				break;
			case MSG_USER_BTN2:
				increment_counter(-1);
				break;
		}
		window_dispatch(window, &msg);
	}
	return 0;
}